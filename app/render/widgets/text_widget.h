#pragma once
#include "../render.h"
#include <string>

class TextWidget : public WidgetLockable {
private:
    std::string text;
    const uint8_t* font;

public:
    TextWidget();
    TextWidget(const uint8_t* font);
    TextWidget(const uint8_t* font, const char* text);
    ~TextWidget() override;

    void render(Canvas* canvas, int16_t x, int16_t y) override;
    std::pair<uint16_t, uint16_t> get_size() override;

    void set_text(const char* text);
    void set_font(const uint8_t* font);
};
