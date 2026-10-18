#include "BarCodeReader.hpp"

#include "BarCodeReaderError.hpp"
#include "BarCodeReaderIdle.hpp"
#include "BarCodeReaderRead.hpp"
#include "BarCodeReaderReset.hpp"

#include <utility>

BarCodeReader::BarCodeReader(Driver& driver) : driver_(driver) {
    changeState(std::make_unique<BarCodeReaderReset>());
}

BarCodeReader::~BarCodeReader() = default;

bool BarCodeReader::initializeComplete() {
    return state_->initializeComplete(*this);
}

bool BarCodeReader::failedToInitialize() {
    return state_->failedToInitialize(*this);
}

bool BarCodeReader::initializeFailed() {
    return state_->initializeFailed(*this);
}

bool BarCodeReader::readRequest() {
    return state_->readRequest(*this);
}

bool BarCodeReader::readCompleteOrTimeout(const std::string& barcode, bool timedOut) {
    return state_->readCompleteOrTimeout(*this, barcode, timedOut);
}

bool BarCodeReader::driverCommError(const std::string& detail) {
    return state_->driverCommError(*this, detail);
}

ReaderState BarCodeReader::state() const {
    return state_->id();
}

const char* BarCodeReader::stateName() const {
    return toString(state());
}

int BarCodeReader::timesEntered(ReaderState id) const {
    return entryCount_[static_cast<std::size_t>(id)];
}

bool BarCodeReader::hasReadResult() const { return hasReadResult_; }

bool BarCodeReader::lastReadTimedOut() const { return lastReadTimedOut_; }

const std::string& BarCodeReader::lastBarcode() const { return lastBarcode_; }

bool BarCodeReader::hasDriverError() const { return hasDriverError_; }

const std::string& BarCodeReader::lastDriverError() const { return lastDriverError_; }

void BarCodeReader::changeState(std::unique_ptr<BarCodeReaderState> next) {
    if (!next) {
        return;
    }
    if (state_) {
        state_->onExit(*this);
    }
    state_ = std::move(next);
    ++entryCount_[static_cast<std::size_t>(state_->id())];
    state_->onEntry(*this);
}

void BarCodeReader::acceptRead(const std::string& barcode, bool timedOut) {
    hasReadResult_ = true;
    lastReadTimedOut_ = timedOut;
    lastBarcode_ = timedOut ? std::string{} : barcode;
}

void BarCodeReader::acceptDriverError(const std::string& detail) {
    hasDriverError_ = true;
    lastDriverError_ = detail.empty() ? std::string{"Driver comm error"} : detail;
}

Driver& BarCodeReader::driver() { return driver_; }
