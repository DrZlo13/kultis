#include "positioned_screen.h"

PositionedScreen::PositionedWidget::PositionedWidget(Widget* widget, int16_t x, int16_t y)
    : widget(widget)
    , x(x)
    , y(y) {
}

PositionedScreen::PositionedWidget::~PositionedWidget() {
    delete widget;
}

PositionedScreen::PositionedScreen() {
}

PositionedScreen::~PositionedScreen() {
    for(auto widget : widgets) {
        delete widget;
    }
}

void PositionedScreen::render(Canvas* canvas, int16_t x, int16_t y) {
    for(auto widget : widgets) {
        if(widget->widget->is_enabled()) {
            int16_t w_x = widget->x;
            int16_t w_y = widget->y;
            int16_t w, h;
            std::tie(w, h) = widget->widget->get_size();
            if(w > 0 && h > 0) {
                WidgetGuard guard(widget->widget);
                widget->widget->render(canvas, x + w_x, y + w_y);
            }
        }
    }
}

bool PositionedScreen::input(InputEvent* input_event) {
    for(auto widget : widgets) {
        if(widget->widget->input(input_event)) {
            return true;
        }
    }
    return false;
}

std::pair<uint16_t, uint16_t> PositionedScreen::get_size() {
    uint16_t width = 0;
    uint16_t height = 0;
    for(auto widget : widgets) {
        int16_t x = widget->x;
        int16_t y = widget->y;
        int16_t w, h;
        std::tie(w, h) = widget->widget->get_size();
        if(x + w > width) {
            width = x + w;
        }
        if(y + h > height) {
            height = y + h;
        }
    }
    return std::make_pair(width, height);
}

Widget* PositionedScreen::add_widget(Widget* widget, int16_t x, int16_t y) {
    Kultis::MutexGuard guard(mutex);
    widgets.push_back(new PositionedWidget(widget, x, y));
    return widget;
}

void PositionedScreen::set_position(Widget* widget, int16_t x, int16_t y) {
    Kultis::MutexGuard guard(mutex);
    for(auto positioned_widget : widgets) {
        if(positioned_widget->widget == widget) {
            positioned_widget->x = x;
            positioned_widget->y = y;
            return;
        }
    }
}