#pragma once

#include "ReaderState.hpp"

#include <string>

class BarCodeReader;

class BarCodeReaderState {
public:
    virtual ~BarCodeReaderState() = default;

    virtual ReaderState id() const = 0;

    virtual void onEntry(BarCodeReader& reader);
    virtual void onExit(BarCodeReader& reader);

    virtual bool initializeComplete(BarCodeReader& reader);
    virtual bool failedToInitialize(BarCodeReader& reader);
    virtual bool initializeFailed(BarCodeReader& reader);
    virtual bool readRequest(BarCodeReader& reader);
    virtual bool readCompleteOrTimeout(BarCodeReader& reader,
                                       const std::string& barcode,
                                       bool timedOut);
    virtual bool driverCommError(BarCodeReader& reader, const std::string& detail);

protected:
    BarCodeReaderState() = default;
};
