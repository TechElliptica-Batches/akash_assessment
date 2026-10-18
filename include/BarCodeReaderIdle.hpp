#pragma once

#include "BarCodeReaderState.hpp"

class BarCodeReaderIdle final : public BarCodeReaderState {
public:
    ReaderState id() const override { return ReaderState::Idle; }

    void onEntry(BarCodeReader& reader) override;

    bool readRequest(BarCodeReader& reader) override;
};
