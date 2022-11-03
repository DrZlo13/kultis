#pragma once
#include "../render.h"

class PositionedScreen : public WidgetLockable {
private:
    Kultis::Mutex mutex;
    struct PositionedWidget {
        Widget* widget;
        int16_t x;
        int16_t y;

        PositionedWidget(Widget* widget, int16_t x, int16_t y);
        ~PositionedWidget();
    };

    std::vector<PositionedWidget*> widgets;

public:
    PositionedScreen();
    ~PositionedScreen() override;

    void render(Canvas* canvas, int16_t x, int16_t y) override;
    bool input(InputEvent* input_event) override;
    std::pair<uint16_t, uint16_t> get_size() override;
    Widget* add_widget(Widget* widget, int16_t x, int16_t y);
    void set_position(Widget* widget, int16_t x, int16_t y);
};
