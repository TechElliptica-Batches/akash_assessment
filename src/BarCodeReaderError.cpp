#include "BarCodeReaderError.hpp"

#include "BarCodeReader.hpp"
#include "BarCodeReaderIdle.hpp"

#include <memory>

void BarCodeReaderError::onEntry(BarCodeReader& reader) {
    reader.driver().faulted();
}

bool BarCodeReaderError::initializeComplete(BarCodeReader& reader) {
    reader.changeState(std::make_unique<BarCodeReaderIdle>());
    return true;
}
