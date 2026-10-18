#pragma once

#include <cstddef>
#include <cstdint>

enum class ReaderState : std::uint8_t {
    Reset = 0,
    Idle = 1,
    Reading = 2,
    Error = 3
};

inline constexpr std::size_t kReaderStateCount = 4;

inline const char* toString(ReaderState state) {
    switch (state) {
    case ReaderState::Reset:
        return "Reset";
    case ReaderState::Idle:
        return "Idle";
    case ReaderState::Reading:
        return "Reading";
    case ReaderState::Error:
        return "Error";
    }
    return "Unknown";
}
