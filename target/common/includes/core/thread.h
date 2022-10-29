#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Kultis {
typedef int32_t (*ThreadCallback)(void* context);

class KThread {
private:
    void* context;

public:
    KThread();
    ~KThread();
    void set_name(const char* name);
    void set_stack_size(size_t stack_size);
    void set_callback(ThreadCallback callback, void* arg);
    void start();
    void join();
};
}