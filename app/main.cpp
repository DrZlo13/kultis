#include <hal/hal.h>
#include <core/core.h>
#include <stdlib.h>
#include "render/render.h"
#include "render/font/fonts.h"

#include "render/widgets/text_widget.h"
#include "render/widgets/positioned_screen.h"

#define TAG "Main"

void main_input_cb(InputEvent* input_event, void* context) {
    Kultis::Queue* queue = (Kultis::Queue*)context;
    queue->send(input_event);
}

int32_t main_thread(void* context) {
    hal_log(LogLevel::Info, TAG, "Main thread started");

    Gui gui;
    PositionedScreen screen;
    TextWidget header(u8g2_font_helvB08_tr, "Header");
    TextWidget text(u8g2_font_haxrcorp4089_tr, "this is a text");

    size_t width = gui.get_width();
    size_t height = gui.get_height();
    uint8_t pixel_x = width / 2;
    uint8_t pixel_y = height / 2;

    screen.add_widget(&header, 0, 0);
    screen.add_widget(&text, pixel_x, pixel_y);
    gui.add_widget(&screen);

    Kultis::Queue queue(10, sizeof(InputEvent));
    hal_input_set_callback(main_input_cb, &queue);

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

                if(pixel_x >= width) {
                    pixel_x = width - 1;
                } else if(pixel_x < 0) {
                    pixel_x = 0;
                }

                if(pixel_y >= height) {
                    pixel_y = height - 1;
                } else if(pixel_y < 0) {
                    pixel_y = 0;
                }

                std::string text_str =
                    "x: " + std::to_string(pixel_x) + " y: " + std::to_string(pixel_y);
                text.set_text(text_str.c_str());
                screen.set_position(&text, pixel_x, pixel_y);
            }
        }

        gui.render();
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
