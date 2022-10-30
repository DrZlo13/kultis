#include <core/delay.h>
#include <thread>

namespace Kultis {
void delay(std::chrono::milliseconds ms) {
    std::this_thread::sleep_for(ms);
}

void delay(std::chrono::microseconds us) {
    std::this_thread::sleep_for(us);
}
}