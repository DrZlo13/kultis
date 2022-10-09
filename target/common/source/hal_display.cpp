#include <hal/hal_display.h>

static HALDisplay* display;

void hal_display_init(void) {
    display = new HALDisplay();
}

HALDisplay* hal_display_get(void) {
    return display;
}