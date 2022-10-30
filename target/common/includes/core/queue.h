#pragma once
#include <stdint.h>
#include <stddef.h>
#include "status.h"
#include <chrono>

namespace Kultis {
class Queue {
private:
    void* context;

public:
    Queue(uint32_t msg_count, uint32_t msg_size);
    ~Queue();
    Status send(void* msg, std::chrono::milliseconds timeout);
    Status receive(void* msg, std::chrono::milliseconds timeout);
    uint32_t get_msg_count();
    uint32_t get_free_space();
    Status reset();
};
}