#include "BarCodeReaderState.hpp"

void BarCodeReaderState::onEntry(BarCodeReader&) {}

void BarCodeReaderState::onExit(BarCodeReader&) {}

bool BarCodeReaderState::initializeComplete(BarCodeReader&) { return false; }

bool BarCodeReaderState::failedToInitialize(BarCodeReader&) { return false; }

bool BarCodeReaderState::initializeFailed(BarCodeReader&) { return false; }

bool BarCodeReaderState::readRequest(BarCodeReader&) { return false; }

bool BarCodeReaderState::readCompleteOrTimeout(BarCodeReader&,
                                              const std::string&,
                                              bool) {
    return false;
}

bool BarCodeReaderState::driverCommError(BarCodeReader&, const std::string&) {
    return false;
}
