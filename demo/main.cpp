#include "BarCodeReader.hpp"
#include "Driver.hpp"

#include <iostream>
#include <string>

namespace {

class PrintingDriver final : public Driver {
public:
    void initialize() override { std::cout << "    driver: initialize\n"; }
    void ready() override { std::cout << "    driver: ready\n"; }
    void read() override { std::cout << "    driver: read\n"; }
    void faulted() override { std::cout << "    driver: faulted\n"; }
};

void show(const BarCodeReader& reader, const std::string& event, bool handled) {
    std::cout << event << (handled ? "" : " (ignored)") << " -> " << reader.stateName();
    if (reader.hasReadResult()) {
        std::cout << "  read=";
        if (reader.lastReadTimedOut()) {
            std::cout << "timeout";
        } else {
            std::cout << '"' << reader.lastBarcode() << '"';
        }
    }
    if (reader.hasDriverError()) {
        std::cout << "  error=\"" << reader.lastDriverError() << '"';
    }
    std::cout << '\n';
}

}

int main() {
    PrintingDriver driver;
    BarCodeReader reader(driver);

    std::cout << "Bar code reader\n";
    std::cout << "start -> " << reader.stateName() << "\n";

    show(reader, "Failed to initialize", reader.failedToInitialize());
    show(reader, "Initialize Complete", reader.initializeComplete());
    show(reader, "read request", reader.readRequest());
    show(reader, "read complete", reader.readCompleteOrTimeout("012345678905", false));
    show(reader, "read request", reader.readRequest());
    show(reader, "Driver comm error", reader.driverCommError("link lost"));
    show(reader, "read request", reader.readRequest());
    show(reader, "Initialize complete", reader.initializeComplete());
    show(reader, "read request", reader.readRequest());
    show(reader, "read complete or timeout", reader.readCompleteOrTimeout("", true));

    return 0;
}
