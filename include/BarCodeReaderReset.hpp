#pragma once

#include "BarCodeReaderState.hpp"

class BarCodeReaderReset final : public BarCodeReaderState {
public:
    ReaderState id() const override { return ReaderState::Reset; }

    void onEntry(BarCodeReader& reader) override;

    bool initializeComplete(BarCodeReader& reader) override;
    bool failedToInitialize(BarCodeReader& reader) override;
    bool initializeFailed(BarCodeReader& reader) override;
};
