#include "BarCodeReaderIdle.hpp"

#include "BarCodeReader.hpp"
#include "BarCodeReaderRead.hpp"

#include <memory>

void BarCodeReaderIdle::onEntry(BarCodeReader& reader) {
    reader.driver().ready();
}

bool BarCodeReaderIdle::readRequest(BarCodeReader& reader) {
    reader.changeState(std::make_unique<BarCodeReaderRead>());
    return true;
}
