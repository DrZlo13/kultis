#include "render.h"

WidgetGuard::WidgetGuard(Widget* widget)
    : widget(widget) {
    widget->lock();
}

WidgetGuard::~WidgetGuard() {
    widget->unlock();
}

void WidgetLockable::lock() {
    mutex.lock();
}

void WidgetLockable::unlock() {
    mutex.unlock();
}

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
    Canvas canvas(display->get_display_buffer(), width, height);
    canvas.fill(false);

    for(Widget* widget : widgets) {
        if(widget->is_enabled()) {
            int16_t w, h;
            std::tie(w, h) = widget->get_size();
            if(w > 0 && h > 0) {
                WidgetGuard guard(widget);
                widget->render(&canvas, 0, 0);
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

Widget* Gui::add_widget(Widget* widget) {
    widgets.push_back(widget);
    return widget;
}

uint16_t Gui::get_width() {
    return width;
}

uint16_t Gui::get_height() {
    return height;
}