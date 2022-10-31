#include <core/mutex.h>
#include <mutex>

using namespace Kultis;

#define get_context() (reinterpret_cast<MutexContext*>(context))

struct MutexContext {
    std::mutex mutex;
};

Mutex::Mutex() {
    this->context = static_cast<void*>(new MutexContext());
}

Mutex::~Mutex() {
    delete get_context();
}

void Mutex::lock() {
    get_context()->mutex.lock();
}

void Mutex::unlock() {
    get_context()->mutex.unlock();
}

MutexGuard::MutexGuard(Mutex& mutex)
    : _mutex(mutex) {
    _mutex.lock();
}

MutexGuard::~MutexGuard() {
    _mutex.unlock();
}
