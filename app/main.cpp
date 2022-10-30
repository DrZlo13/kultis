#include <hal/hal.h>
#include <core/core.h>
#include <stdlib.h>

#define TAG "Main"

void main_input_cb(InputEvent* input_event, void* context) {
    Kultis::Queue* queue = (Kultis::Queue*)context;
    queue->send(input_event);
}

const char* input_get_type_name(InputType type) {
    switch(type) {
    case InputType::Press:
        return "Press";
    case InputType::Release:
        return "Release";
    case InputType::Short:
        return "Short";
    case InputType::Long:
        return "Long";
    case InputType::Repeat:
        return "Repeat";
    default:
        return "Unknown";
    }
}

const char* input_get_key_name(InputKey key) {
    switch(key) {
    case InputKey::Up:
        return "Up";
    case InputKey::Down:
        return "Down";
    case InputKey::Right:
        return "Right";
    case InputKey::Left:
        return "Left";
    case InputKey::Ok:
        return "Ok";
    case InputKey::Back:
        return "Back";
    default:
        return "Unknown";
    }
}

int32_t main_thread(void* context) {
    hal_log(LogLevel::Info, TAG, "Main thread started");

    HALDisplay* display = hal_display_get();
    size_t width = display->get_width();
    size_t height = display->get_height();
    Kultis::Queue queue(10, sizeof(InputEvent));
    hal_input_set_callback(main_input_cb, &queue);

    uint8_t pixel_x = width / 2;
    uint8_t pixel_y = height / 2;

    while(true) {
        InputEvent input_event;
        Kultis::Status status = queue.receive(&input_event, std::chrono::milliseconds(20));

        if(status == Kultis::Status::OK) {
            if(input_event.type == InputType::Press || input_event.type == InputType::Repeat) {
                if(input_event.key == InputKey::Up) {
                    pixel_y -= 1;
                } else if(input_event.key == InputKey::Down) {
                    pixel_y += 1;
                } else if(input_event.key == InputKey::Left) {
                    pixel_x -= 1;
                } else if(input_event.key == InputKey::Right) {
                    pixel_x += 1;
                }
            }

            hal_log(
                LogLevel::Info,
                TAG,
                "Input event: %s %s",
                input_get_type_name(input_event.type),
                input_get_key_name(input_event.key));
        }

        DisplayBuffer* buffer = display->get_display_buffer();
        buffer->fill(false);
        buffer->set_pixel(pixel_x, pixel_y, true);
        display->commit_display_buffer(true);
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
