cmake_minimum_required(VERSION 3.16)
project(BarCodeReader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(barcode
    src/BarCodeReaderState.cpp
    src/BarCodeReader.cpp
    src/BarCodeReaderReset.cpp
    src/BarCodeReaderIdle.cpp
    src/BarCodeReaderRead.cpp
    src/BarCodeReaderError.cpp
)
target_include_directories(barcode PUBLIC include)
target_compile_options(barcode PRIVATE -Wall -Wextra -Wpedantic)

add_executable(barcode_tests tests/state_machine_test.cpp)
target_link_libraries(barcode_tests PRIVATE barcode)
target_compile_options(barcode_tests PRIVATE -Wall -Wextra -Wpedantic)

add_executable(barcode_demo demo/main.cpp)
target_link_libraries(barcode_demo PRIVATE barcode)
target_compile_options(barcode_demo PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_test(NAME state_machine COMMAND barcode_tests)
