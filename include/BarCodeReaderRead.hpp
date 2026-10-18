#pragma once

#include "BarCodeReaderState.hpp"

class BarCodeReaderRead final : public BarCodeReaderState {
public:
    ReaderState id() const override { return ReaderState::Reading; }

    void onEntry(BarCodeReader& reader) override;

    bool readCompleteOrTimeout(BarCodeReader& reader,
                               const std::string& barcode,
                               bool timedOut) override;
    bool driverCommError(BarCodeReader& reader, const std::string& detail) override;
};
