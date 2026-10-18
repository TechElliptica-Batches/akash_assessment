#pragma once

#include "BarCodeReaderState.hpp"
#include "Driver.hpp"
#include "Imager.hpp"
#include "ReaderState.hpp"

#include <array>
#include <memory>
#include <string>

class BarCodeReaderReset;
class BarCodeReaderIdle;
class BarCodeReaderRead;
class BarCodeReaderError;

class BarCodeReader : public Imager {
public:
    explicit BarCodeReader(Driver& driver);
    ~BarCodeReader() override;

    BarCodeReader(const BarCodeReader&) = delete;
    BarCodeReader& operator=(const BarCodeReader&) = delete;
    BarCodeReader(BarCodeReader&&) = delete;
    BarCodeReader& operator=(BarCodeReader&&) = delete;

    bool initializeComplete();
    bool failedToInitialize();
    bool initializeFailed();
    bool readRequest();
    bool readCompleteOrTimeout(const std::string& barcode, bool timedOut);
    bool driverCommError(const std::string& detail = {});

    ReaderState state() const;
    const char* stateName() const;
    int timesEntered(ReaderState id) const;

    bool hasReadResult() const;
    bool lastReadTimedOut() const;
    const std::string& lastBarcode() const;

    bool hasDriverError() const;
    const std::string& lastDriverError() const;

private:
    friend class BarCodeReaderReset;
    friend class BarCodeReaderIdle;
    friend class BarCodeReaderRead;
    friend class BarCodeReaderError;

    void changeState(std::unique_ptr<BarCodeReaderState> next);
    void acceptRead(const std::string& barcode, bool timedOut);
    void acceptDriverError(const std::string& detail);
    Driver& driver();

    Driver& driver_;
    std::unique_ptr<BarCodeReaderState> state_;
    std::array<int, kReaderStateCount> entryCount_{};
    std::string lastBarcode_;
    bool hasReadResult_ = false;
    bool lastReadTimedOut_ = false;
    std::string lastDriverError_;
    bool hasDriverError_ = false;
};
