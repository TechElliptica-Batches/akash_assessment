#pragma once

#include "BarCodeReaderState.hpp"

class BarCodeReaderError final : public BarCodeReaderState {
public:
    ReaderState id() const override { return ReaderState::Error; }

    void onEntry(BarCodeReader& reader) override;

    bool initializeComplete(BarCodeReader& reader) override;
};
