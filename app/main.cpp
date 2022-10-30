#include <hal/hal.h>
#include <core/core.h>
#include <stdlib.h>

int32_t main_thread(void* context) {
    HALDisplay* display = hal_display_get();
    size_t width = display->get_width();
    size_t height = display->get_height();

    while(true) {
        DisplayBuffer* buffer = display->get_display_buffer();
        buffer->fill(false);
        for(size_t i = 0; i < 1000; i++) {
            // set random pixel
            buffer->set_pixel(rand() % width, rand() % height, true);
        }
        display->commit_display_buffer(true);
        Kultis::delay(std::chrono::milliseconds(20));
    }
}

int main(int argc, char** argv) {
    hal_pre_init();
    hal_init();

    Kultis::Thread* thread = new Kultis::Thread();
    thread->set_callback(main_thread, nullptr);
    thread->start();

    hal_post_init();

    // thread->join();
    return 0;
}
