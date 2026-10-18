# Barcode reader state machine

C++ version of the state machine from the assessment.

The reader starts in Reset. From there it can go to Idle, Reading, or Error depending on what happens (init works, init fails, someone asks for a read, the read finishes, or the driver loses communication).

## How to run

```sh
make test
make demo
```

`make test` builds and runs the checks. `make demo` prints a short session so you can see the states change.

If you have CMake:

```sh
cmake -S . -B build
cmake --build build
./build/barcode_demo
```

## What the states do

- **Reset** — trying to initialize. "Failed to initialize" tries again. "Initialize failed" goes to Error. "Initialize Complete" goes to Idle.
- **Idle** — waiting. A read request goes to Reading. Anything else is ignored.
- **Reading** — a read is in progress. Success or timeout goes back to Idle. A driver comm error goes to Error.
- **Error** — stuck until "Initialize complete", which goes back to Idle.

`BarCodeReader` is the main class. It holds the current state and uses a `Driver` for init, ready, read, and fault. The four state classes are `BarCodeReaderReset`, `BarCodeReaderIdle`, `BarCodeReaderRead`, and `BarCodeReaderError`.
