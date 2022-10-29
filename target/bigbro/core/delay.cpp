#include <core/delay.h>
#include <thread>

namespace Kultis {
void delay_ms(uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void delay_us(uint32_t us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}
}