#pragma once
#include <stdint.h>

namespace Kultis {
enum class Status : uint32_t {
    OK = 0,
    Error = (uint32_t)-1,
    ErrorTimeout = (uint32_t)-2,
    ErrorResource = (uint32_t)-3,
    ErrorParameter = (uint32_t)-4,
    ErrorNoMemory = (uint32_t)-5,
    ErrorISR = (uint32_t)-6,
    Reserved = 0x7FFFFFFF,
};
}