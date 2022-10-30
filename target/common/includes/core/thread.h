#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Kultis {
typedef int32_t (*ThreadCallback)(void* context);

class Thread {
private:
    void* context;

public:
    Thread();
    ~Thread();
    void set_name(const char* name);
    void set_stack_size(size_t stack_size);
    void set_callback(ThreadCallback callback, void* arg);
    void start();
    void join();
};
}