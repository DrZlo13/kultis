#include "text_widget.h"

TextWidget::TextWidget() {
    this->font = nullptr;
    this->text = "";
}

TextWidget::TextWidget(const uint8_t* font) {
    this->font = font;
    this->text = "";
}

TextWidget::TextWidget(const uint8_t* font, const char* text) {
    this->font = font;
    this->text = text;
}

TextWidget::~TextWidget() = default;

void TextWidget::render(Canvas* canvas, int16_t x, int16_t y) {
    canvas->text_set_font(font);
    canvas->text(x, y, text.c_str());
}

std::pair<uint16_t, uint16_t> TextWidget::get_size() {
    return std::make_pair(128, 64);
}

void TextWidget::set_text(const char* text) {
    WidgetGuard guard(this);
    this->text = text;
}

void TextWidget::set_font(const uint8_t* font) {
    WidgetGuard guard(this);
    this->font = font;
}
