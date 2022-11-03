#pragma once
#include <stdint.h>
#include <utility>
#include <vector>
#include <hal/hal_input.h>
#include <hal/hal_display.h>
#include <core/core.h>

class Canvas {
private:
    DisplayBuffer* buffer;
    const uint16_t width;
    const uint16_t height;
    const uint8_t* font;

public:
    Canvas(DisplayBuffer* buffer, uint16_t width, uint16_t height);
    void set_pixel(uint16_t x, uint16_t y, bool value);
    void fill(bool value);

    void text_set_font(const uint8_t* font);
    void text(uint16_t x, uint16_t y, const char* text);
};

class Widget {
public:
    virtual ~Widget() = default;

    virtual void lock(){};

    virtual void unlock(){};

    virtual void render(Canvas* canvas, int16_t x, int16_t y){};

    virtual bool input(InputEvent* input_event) {
        return false;
    }

    virtual std::pair<uint16_t, uint16_t> get_size() {
        return std::make_pair(UINT16_MAX, UINT16_MAX);
    }

    virtual bool is_enabled() {
        return true;
    }
};

class WidgetGuard {
private:
    Widget* widget;

public:
    WidgetGuard(Widget* widget);
    ~WidgetGuard();
};

class WidgetLockable : public Widget {
private:
    Kultis::Mutex mutex;

public:
    void lock() override;
    void unlock() override;
};

class Gui {
private:
    std::vector<Widget*> widgets;
    HALDisplay* display;
    const uint16_t width;
    const uint16_t height;

public:
    Gui();
    ~Gui();
    void render();
    void input(InputEvent* input_event);
    Widget* add_widget(Widget* widget);
    uint16_t get_width();
    uint16_t get_height();
};
