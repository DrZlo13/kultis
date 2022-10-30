#include <core/thread.h>
#include <thread>

using namespace Kultis;

#define get_context() (reinterpret_cast<ThreadContext*>(context))

struct ThreadContext {
    const char* name;
    size_t stack_size;
    ThreadCallback callback;
    void* arg;
    std::thread thread;
};

Thread::Thread() {
    this->context = static_cast<void*>(new ThreadContext());
}

Thread::~Thread() {
    delete get_context();
}

void Thread::set_name(const char* name) {
    get_context()->name = name;
}

void Thread::set_stack_size(size_t stack_size) {
    get_context()->stack_size = stack_size;
}

void Thread::set_callback(ThreadCallback callback, void* arg) {
    get_context()->callback = callback;
    get_context()->arg = arg;
}

void Thread::start() {
    get_context()->thread = std::thread(get_context()->callback, get_context()->arg);
}

void Thread::join() {
    get_context()->thread.join();
}