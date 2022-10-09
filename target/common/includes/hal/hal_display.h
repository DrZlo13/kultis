#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

class DisplayBuffer {
public:
    void set_pixel(size_t x, size_t y, bool value);
    void fill(bool value);
};

class HALDisplay {
private:
    /* data */
public:
    HALDisplay();
    ~HALDisplay();

    size_t get_width();
    size_t get_height();
    DisplayBuffer* get_display_buffer();
    void commit_display_buffer(bool redraw);
};

void hal_display_init(void);

HALDisplay* hal_display_get(void);