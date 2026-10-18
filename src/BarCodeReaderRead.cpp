#include "BarCodeReaderRead.hpp"

#include "BarCodeReader.hpp"
#include "BarCodeReaderError.hpp"
#include "BarCodeReaderIdle.hpp"

#include <memory>

void BarCodeReaderRead::onEntry(BarCodeReader& reader) {
    reader.driver().read();
}

bool BarCodeReaderRead::readCompleteOrTimeout(BarCodeReader& reader,
                                              const std::string& barcode,
                                              bool timedOut) {
    reader.acceptRead(barcode, timedOut);
    reader.changeState(std::make_unique<BarCodeReaderIdle>());
    return true;
}

bool BarCodeReaderRead::driverCommError(BarCodeReader& reader, const std::string& detail) {
    reader.acceptDriverError(detail);
    reader.changeState(std::make_unique<BarCodeReaderError>());
    return true;
}
