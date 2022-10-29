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

KThread::KThread() {
    this->context = static_cast<void*>(new ThreadContext());
}

KThread::~KThread() {
    delete get_context();
}

void KThread::set_name(const char* name) {
    get_context()->name = name;
}

void KThread::set_stack_size(size_t stack_size) {
    get_context()->stack_size = stack_size;
}

void KThread::set_callback(ThreadCallback callback, void* arg) {
    get_context()->callback = callback;
    get_context()->arg = arg;
}

void KThread::start() {
    get_context()->thread = std::thread(get_context()->callback, get_context()->arg);
}

void KThread::join() {
    get_context()->thread.join();
}