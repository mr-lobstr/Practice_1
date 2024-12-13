#ifndef WRITE_READ_SEMAPHORE_H_GUARD
#define WRITE_READ_SEMAPHORE_H_GUARD

#include <mutex>
#include <condition_variable>

class WRSemaphore {
public:
    bool is_free() noexcept {
        std::unique_lock<std::mutex> lock (mtx);
        return not writing and readerCnt == 0;
    }

    void start_reading() {
        std::unique_lock<std::mutex> lock (mtx);

        cv.wait(lock, [this] { return writing == false; });

        ++readerCnt;
    }

    void start_writing() {
        std::unique_lock<std::mutex> lock (mtx);

        cv.wait(lock, [this] { return readerCnt == 0; });

        writing = true;
    }

    void finish_reading() {
        std::unique_lock<std::mutex> lock (mtx);
        --readerCnt;
        cv.notify_one();
    }

    void finish_writing() {
        std::unique_lock<std::mutex> lock (mtx);
        writing = false;
        cv.notify_all();
    }

private:
    std::mutex mtx{};
    std::condition_variable cv{};
    
    size_t readerCnt = 0;
    bool writing = false;
};


class WriteLock {
public:
    WriteLock (WRSemaphore& sema_)
        : sema (sema_)
    {
        sema.start_writing();
    }

    ~WriteLock() noexcept {
        sema.finish_writing();
    }

private:
    WRSemaphore& sema;
};


class ReadLock {
public:
    ReadLock (WRSemaphore& sema_)
        : sema (sema_)
    {
        sema.start_reading();
    }

    ~ReadLock() noexcept {
        sema.finish_reading();
    }

private:
    WRSemaphore& sema;
};

#endif