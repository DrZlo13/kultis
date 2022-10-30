#pragma once
#include <stdint.h>
#include <chrono>

namespace Kultis {
void delay(std::chrono::milliseconds ms);
void delay(std::chrono::microseconds us);
}