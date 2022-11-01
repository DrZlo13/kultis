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
    virtual void render(Canvas* canvas) = 0;
    virtual bool input(InputEvent* input_event) = 0;
    virtual std::pair<uint16_t, uint16_t> get_size() = 0;
    virtual std::pair<int16_t, int16_t> get_position() = 0;
    virtual bool is_enabled() = 0;
};

class Gui {
private:
    std::vector<Widget*> widgets;
    HALDisplay* display;
    const uint16_t width;
    const uint16_t height;
    Kultis::Mutex mutex;

public:
    Gui();
    ~Gui();
    void render();
    void input(InputEvent* input_event);
    void add_widget(Widget* widget);
};
