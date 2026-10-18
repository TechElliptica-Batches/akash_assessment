#include "BarCodeReaderReset.hpp"

#include "BarCodeReader.hpp"
#include "BarCodeReaderError.hpp"
#include "BarCodeReaderIdle.hpp"

#include <memory>

void BarCodeReaderReset::onEntry(BarCodeReader& reader) {
    reader.driver().initialize();
}

bool BarCodeReaderReset::initializeComplete(BarCodeReader& reader) {
    reader.changeState(std::make_unique<BarCodeReaderIdle>());
    return true;
}

bool BarCodeReaderReset::failedToInitialize(BarCodeReader& reader) {
    reader.changeState(std::make_unique<BarCodeReaderReset>());
    return true;
}

bool BarCodeReaderReset::initializeFailed(BarCodeReader& reader) {
    reader.changeState(std::make_unique<BarCodeReaderError>());
    return true;
}
