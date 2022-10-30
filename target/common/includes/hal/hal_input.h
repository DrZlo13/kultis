#pragma once
#include <stdint.h>

enum class InputType : uint8_t {
    Press,
    Release,
    Short,
    Long,
    Repeat,
};

enum class InputKey : uint8_t {
    Up,
    Down,
    Right,
    Left,
    Ok,
    Back,
};

struct InputEvent {
    InputType type;
    InputKey key;
};

typedef void (*InputCallback)(InputEvent* input_event, void* context);

void hal_input_init(void);

void hal_input_set_callback(InputCallback callback, void* context);