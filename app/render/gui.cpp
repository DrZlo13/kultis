#include "render.h"

Gui::Gui()
    : display(hal_display_get())
    , width(display->get_width())
    , height(display->get_height()) {
}

Gui::~Gui() {
    for(Widget* widget : widgets) {
        delete widget;
    }
}

void Gui::render() {
    Kultis::MutexGuard lock(mutex);
    Canvas canvas(display->get_display_buffer(), width, height);

    for(Widget* widget : widgets) {
        if(widget->is_enabled()) {
            int16_t x, y, w, h;
            std::tie(x, y) = widget->get_position();
            std::tie(w, h) = widget->get_size();
            if(x + w > 0 && x < width && y + h > 0 && y < height) {
                widget->render(&canvas);
            }
        }
    }

    display->commit_display_buffer(true);
}

void Gui::input(InputEvent* input_event) {
    for(Widget* widget : widgets) {
        if(widget->input(input_event)) {
            break;
        }
    }
}

void Gui::add_widget(Widget* widget) {
    widgets.push_back(widget);
}