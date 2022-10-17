#include <hal/hal.h>
#include <thread>
#include <stdlib.h>

void main_thread(void) {
    HALDisplay* display = hal_display_get();

    while(true) {
        DisplayBuffer* buffer = display->get_display_buffer();
        buffer->fill(false);
        for(size_t i = 0; i < 100; i++) {
            // set random pixel
            buffer->set_pixel(rand() % 128, rand() % 64, true);
        }
        display->commit_display_buffer(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

int main(int argc, char** argv) {
    hal_pre_init();
    hal_init();

    std::thread t(main_thread);

    hal_post_init();

    return 0;
}
