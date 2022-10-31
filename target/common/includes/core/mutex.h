#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Kultis {
class Mutex {
private:
    void* context;

public:
    Mutex();
    ~Mutex();
    void lock();
    void unlock();
};

class MutexGuard {
private:
    Mutex& _mutex;

public:
    MutexGuard(Mutex& mutex);
    ~MutexGuard();
};
}