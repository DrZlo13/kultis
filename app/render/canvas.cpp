#include "render.h"
#include "font/u8g2_font_render.h"

static void draw_pixel_fg(uint8_t x, uint8_t y, void* context) {
    Canvas* canvas = (Canvas*)context;
    canvas->set_pixel(x, y, true);
}

static void draw_pixel_bg(uint8_t x, uint8_t y, void* context) {
    Canvas* canvas = (Canvas*)context;
    canvas->set_pixel(x, y, false);
}

Canvas::Canvas(DisplayBuffer* buffer, uint16_t width, uint16_t height)
    : buffer(buffer)
    , width(width)
    , height(height) {
}

void Canvas::set_pixel(uint16_t x, uint16_t y, bool value) {
    if(x < width && y < height) {
        buffer->set_pixel(x, y, value);
    }
}

void Canvas::fill(bool value) {
    buffer->fill(value);
}

void Canvas::text_set_font(const uint8_t* font) {
    this->font = font;
}

void Canvas::text(uint16_t x, uint16_t y, const char* text) {
    U8G2FontRender_t render = U8G2FontRender(this->font, draw_pixel_fg, draw_pixel_bg, this);
    U8G2FontRender_Print(&render, x, y, text);
}