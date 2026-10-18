#include "BarCodeReader.hpp"
#include "Driver.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace {

class RecordingDriver final : public Driver {
public:
    std::vector<std::string> log;

    void initialize() override { log.push_back("initialize"); }
    void ready() override { log.push_back("ready"); }
    void read() override { log.push_back("read"); }
    void faulted() override { log.push_back("faulted"); }
};

int g_failures = 0;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++g_failures;
    }
}

void expectState(const BarCodeReader& reader, ReaderState expected, const std::string& message) {
    if (reader.state() != expected) {
        std::cerr << "FAIL: " << message << " (actual " << reader.stateName() << ")\n";
        ++g_failures;
    }
}

void expectLog(const RecordingDriver& driver,
               const std::vector<std::string>& expected,
               const std::string& message) {
    if (driver.log != expected) {
        std::cerr << "FAIL: " << message << "\n  actual:";
        for (const auto& entry : driver.log) {
            std::cerr << ' ' << entry;
        }
        std::cerr << '\n';
        ++g_failures;
    }
}

void testStartsInReset() {
    RecordingDriver driver;
    BarCodeReader reader(driver);
    expectState(reader, ReaderState::Reset, "reader starts in Reset");
    expect(reader.timesEntered(ReaderState::Reset) == 1, "Reset entered once at start");
    expect(reader.timesEntered(ReaderState::Idle) == 0, "Idle not entered at start");
    expect(!reader.hasReadResult(), "no read result at start");
    expect(!reader.hasDriverError(), "no driver error at start");
    expectLog(driver, {"initialize"}, "Reset entry initializes the driver");
}

void testInitializeComplete() {
    RecordingDriver driver;
    BarCodeReader reader(driver);
    expect(reader.initializeComplete(), "Reset handles Initialize Complete");
    expectState(reader, ReaderState::Idle, "Initialize Complete goes to Idle");
    expectLog(driver, {"initialize", "ready"}, "Idle entry marks the driver ready");
}

void testFailedToInitializeRetries() {
    RecordingDriver driver;
    BarCodeReader reader(driver);
    expect(reader.failedToInitialize(), "Reset handles Failed to initialize");
    expectState(reader, ReaderState::Reset, "Failed to initialize stays in Reset");
    expect(reader.timesEntered(ReaderState::Reset) == 2, "self-transition re-enters Reset");
    expect(reader.failedToInitialize(), "second Failed to initialize is handled");
    expect(reader.timesEntered(ReaderState::Reset) == 3, "each retry re-enters Reset");
    expectLog(driver, {"initialize", "initialize", "initialize"}, "each Reset entry retries init");

    expect(reader.initializeComplete(), "init can still complete after retries");
    expectState(reader, ReaderState::Idle, "complete after retry reaches Idle");
}

void testInitializeFailed() {
    RecordingDriver driver;
    BarCodeReader reader(driver);
    expect(reader.initializeFailed(), "Reset handles Initialize failed");
    expectState(reader, ReaderState::Error, "Initialize failed goes to Error");
    expect(reader.timesEntered(ReaderState::Error) == 1, "Error entered once");
    expectLog(driver, {"initialize", "faulted"}, "Error entry faults the driver");
}

void testErrorRecoversToIdle() {
    RecordingDriver driver;
    BarCodeReader reader(driver);
    reader.initializeFailed();
    expect(reader.initializeComplete(), "Error handles Initialize complete");
    expectState(reader, ReaderState::Idle, "Error recovers directly to Idle");
    expect(reader.timesEntered(ReaderState::Reset) == 1, "recovery does not re-enter Reset");
    expectLog(driver, {"initialize", "faulted", "ready"}, "recovery readies the driver");
}

void testReadSuccessAndReplacement() {
    RecordingDriver driver;
    BarCodeReader reader(driver);
    reader.initializeComplete();

    expect(reader.readRequest(), "Idle handles read request");
    expectState(reader, ReaderState::Reading, "read request goes to Reading");
    expect(!reader.hasReadResult(), "request alone does not finish a read");

    expect(reader.readCompleteOrTimeout("ABC123", false), "Reading handles read complete");
    expectState(reader, ReaderState::Idle, "read complete returns to Idle");
    expect(reader.hasReadResult(), "successful read stores a result");
    expect(!reader.lastReadTimedOut(), "successful read is not a timeout");
    expect(reader.lastBarcode() == "ABC123", "barcode is stored");

    expect(reader.readRequest(), "second read request is handled");
    expectState(reader, ReaderState::Reading, "second request reads again");
    expect(reader.lastBarcode() == "ABC123", "previous barcode is kept until the new read finishes");
    expect(reader.readCompleteOrTimeout("XYZ", false), "second read completes");
    expect(reader.lastBarcode() == "XYZ", "new barcode replaces the previous one");
    expectLog(driver,
              {"initialize", "ready", "read", "ready", "read", "ready"},
              "read cycles notify the driver on each entry");
}

void testReadTimeout() {
    RecordingDriver driver;
    BarCodeReader reader(driver);
    reader.initializeComplete();
    reader.readRequest();
    expect(reader.readCompleteOrTimeout("IGNORED", true), "Reading handles timeout");
    expectState(reader, ReaderState::Idle, "timeout returns to Idle");
    expect(reader.hasReadResult(), "timeout is a finished read");
    expect(reader.lastReadTimedOut(), "timeout flag is set");
    expect(reader.lastBarcode().empty(), "timeout discards the barcode");
}

void testDriverCommError() {
    RecordingDriver driver;
    BarCodeReader reader(driver);
    reader.initializeComplete();
    reader.readRequest();
    reader.readCompleteOrTimeout("KEEP", false);

    expect(reader.readRequest(), "read after a good scan");
    expect(reader.driverCommError("link lost"), "Reading handles Driver comm error");
    expectState(reader, ReaderState::Error, "comm error goes to Error");
    expect(reader.hasDriverError(), "comm error is recorded");
    expect(reader.lastDriverError() == "link lost", "comm error detail is stored");
    expect(reader.lastBarcode() == "KEEP", "a failed read does not erase the previous barcode");
    expect(!reader.lastReadTimedOut(), "previous successful read stays successful");

    RecordingDriver bare;
    BarCodeReader other(bare);
    other.initializeComplete();
    other.readRequest();
    expect(other.driverCommError(), "comm error may omit a detail");
    expect(other.lastDriverError() == "Driver comm error", "empty detail gets the event name");
}

void testIgnoredEventsDoNotReenter() {
    RecordingDriver driver;
    BarCodeReader reader(driver);

    expect(!reader.readRequest(), "Reset ignores read request");
    expect(!reader.readCompleteOrTimeout("NO", false), "Reset ignores read complete");
    expect(!reader.driverCommError("nope"), "Reset ignores Driver comm error");
    expectState(reader, ReaderState::Reset, "ignored events leave Reset");
    expect(reader.timesEntered(ReaderState::Reset) == 1, "ignored events do not re-enter Reset");
    expect(!reader.hasReadResult(), "ignored read complete stores nothing");
    expect(!reader.hasDriverError(), "ignored comm error stores nothing");

    reader.initializeComplete();
    const int idleEntries = reader.timesEntered(ReaderState::Idle);
    expect(!reader.initializeComplete(), "Idle ignores Initialize complete");
    expect(!reader.failedToInitialize(), "Idle ignores Failed to initialize");
    expect(!reader.initializeFailed(), "Idle ignores Initialize failed");
    expect(!reader.readCompleteOrTimeout("NO", false), "Idle ignores read complete");
    expect(!reader.driverCommError("nope"), "Idle ignores Driver comm error");
    expectState(reader, ReaderState::Idle, "ignored events leave Idle");
    expect(reader.timesEntered(ReaderState::Idle) == idleEntries, "Idle self-loop does not re-enter");

    reader.readRequest();
    const int readingEntries = reader.timesEntered(ReaderState::Reading);
    expect(!reader.initializeComplete(), "Reading ignores Initialize complete");
    expect(!reader.failedToInitialize(), "Reading ignores Failed to initialize");
    expect(!reader.initializeFailed(), "Reading ignores Initialize failed");
    expect(!reader.readRequest(), "Reading ignores another read request");
    expectState(reader, ReaderState::Reading, "ignored events leave Reading");
    expect(reader.timesEntered(ReaderState::Reading) == readingEntries,
           "Reading does not re-enter on ignored events");

    reader.driverCommError("boom");
    const int errorEntries = reader.timesEntered(ReaderState::Error);
    expect(!reader.failedToInitialize(), "Error ignores Failed to initialize");
    expect(!reader.initializeFailed(), "Error ignores Initialize failed");
    expect(!reader.readRequest(), "Error ignores read request");
    expect(!reader.readCompleteOrTimeout("NO", true), "Error ignores read complete");
    expect(!reader.driverCommError("again"), "Error ignores another comm error");
    expectState(reader, ReaderState::Error, "ignored events leave Error");
    expect(reader.timesEntered(ReaderState::Error) == errorEntries, "Error self-loop does not re-enter");
    expect(reader.lastDriverError() == "boom", "ignored comm error does not replace the first one");
    expectLog(driver,
              {"initialize", "ready", "read", "faulted"},
              "ignored events do not notify the driver");
}

void testSharedDriverOutlivesReader() {
    RecordingDriver driver;
    {
        BarCodeReader first(driver);
        BarCodeReader second(driver);
        expect(driver.log.size() == 2, "each reader initializes the shared driver");
    }
    expect(driver.log.size() == 2, "destroying readers does not touch the aggregated driver");
}

void testDeleteThroughImager() {
    RecordingDriver driver;
    Imager* imager = new BarCodeReader(driver);
    delete imager;
    expect(driver.log.size() == 1, "reader constructed through Imager still enters Reset");
}

void testFullSession() {
    RecordingDriver driver;
    BarCodeReader reader(driver);

    expect(reader.failedToInitialize(), "session retry");
    expectState(reader, ReaderState::Reset, "session still Reset");
    expect(reader.initializeComplete(), "session init");
    expectState(reader, ReaderState::Idle, "session Idle");
    expect(reader.readRequest(), "session read");
    expect(reader.readCompleteOrTimeout("012345", false), "session scan");
    expect(reader.lastBarcode() == "012345", "session barcode");
    expect(reader.readRequest(), "session second read");
    expect(reader.driverCommError("cable unplugged"), "session comm error");
    expectState(reader, ReaderState::Error, "session Error");
    expect(!reader.readRequest(), "session cannot read while in Error");
    expect(reader.initializeComplete(), "session recover");
    expectState(reader, ReaderState::Idle, "session recovered Idle");
    expect(reader.readRequest(), "session read after recovery");
    expect(reader.readCompleteOrTimeout("", true), "session timeout");
    expectState(reader, ReaderState::Idle, "session idle after timeout");
    expect(reader.lastReadTimedOut(), "session timed out");
    expect(reader.lastBarcode().empty(), "session timeout clears the code");

    expectLog(driver,
              {"initialize", "initialize", "ready", "read", "ready", "read", "faulted", "ready", "read", "ready"},
              "session driver trace");
}

}

int main() {
    testStartsInReset();
    testInitializeComplete();
    testFailedToInitializeRetries();
    testInitializeFailed();
    testErrorRecoversToIdle();
    testReadSuccessAndReplacement();
    testReadTimeout();
    testDriverCommError();
    testIgnoredEventsDoNotReenter();
    testSharedDriverOutlivesReader();
    testDeleteThroughImager();
    testFullSession();

    if (g_failures != 0) {
        std::cerr << g_failures << " assertion(s) failed\n";
        return 1;
    }
    std::cout << "All state machine tests passed\n";
    return 0;
}
