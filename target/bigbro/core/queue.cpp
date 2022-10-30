#include <core/queue.h>
#include <queue>
#include <mutex>

namespace Kultis {
class QueueInstance {
private:
    uint32_t msg_size;

    mutable std::mutex mutex;
    std::queue<void*> queue;
    std::condition_variable notifier;

    void push(void* msg) {
        void* msg_copy = malloc(msg_size);
        memcpy(msg_copy, msg, msg_size);
        queue.push(msg_copy);
    }

    void pop(void* msg) {
        void* msg_copy = queue.front();
        memcpy(msg, msg_copy, msg_size);
        free(msg_copy);
        queue.pop();
    }

    void pop_and_free() {
        void* msg_copy = queue.front();
        free(msg_copy);
        queue.pop();
    }

public:
    QueueInstance(uint32_t _msg_size) {
        msg_size = _msg_size;
    }

    ~QueueInstance() {
        reset();
    }

    Status send(void* msg) {
        {
            std::unique_lock<std::mutex> lock(mutex);
            push(msg);
        }
        notifier.notify_one();
        return Status::OK;
    }

    Status receive(void* msg) {
        std::unique_lock<std::mutex> lock(mutex);
        while(queue.empty()) {
            notifier.wait(lock);
        }
        pop(msg);
        return Status::OK;
    }

    Status receive(void* msg, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex);
        Status status = Status::ErrorTimeout;
        if(notifier.wait_for(lock, timeout, [this] { return !queue.empty(); })) {
            pop(msg);
            status = Status::OK;
        }
        return status;
    }

    uint32_t get_msg_count() {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.size() / sizeof(void*);
    }

    uint32_t get_free_space() {
        std::unique_lock<std::mutex> lock(mutex);
        return UINT32_MAX;
    }

    Status reset() {
        std::unique_lock<std::mutex> lock(mutex);
        while(!queue.empty()) {
            pop_and_free();
        }
        return Status::OK;
    }
};

Queue::Queue(uint32_t msg_count, uint32_t msg_size) {
    this->context = static_cast<void*>(new QueueInstance(msg_size));
}

Queue::~Queue() {
    delete static_cast<QueueInstance*>(this->context);
}

Status Queue::send(void* msg) {
    QueueInstance* queue = static_cast<QueueInstance*>(this->context);
    return queue->send(msg);
}

Status Queue::send(void* msg, std::chrono::milliseconds timeout) {
    QueueInstance* queue = static_cast<QueueInstance*>(this->context);
    return queue->send(msg);
}

Status Queue::receive(void* msg) {
    QueueInstance* queue = static_cast<QueueInstance*>(this->context);
    return queue->receive(msg);
}

Status Queue::receive(void* msg, std::chrono::milliseconds timeout) {
    QueueInstance* queue = static_cast<QueueInstance*>(this->context);
    return queue->receive(msg, timeout);
}

uint32_t Queue::get_msg_count() {
    QueueInstance* queue = static_cast<QueueInstance*>(this->context);
    return queue->get_msg_count();
}

uint32_t Queue::get_free_space() {
    return UINT32_MAX;
}

Status Queue::reset() {
    QueueInstance* queue = static_cast<QueueInstance*>(this->context);
    return queue->reset();
}
}