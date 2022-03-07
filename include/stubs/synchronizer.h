#ifndef __synchronizer_stub_h
#define __synchronizer_stub_h

#include <stubs/stub.h>

#define MUTEX_ID EPOS::S::MUTEX_ID
#define SEMAPHORE_ID EPOS::S::SEMAPHORE_ID

class Semaphore_Stub : public Stub_Common
{
public:
    Semaphore_Stub(int v = 1) {
        _semaphore = reinterpret_cast<void *>(stub_syscall(SEMAPHORE_ID, Message::CONSTRUCTOR1, v));
    }

    ~Semaphore_Stub() {
        stub_syscall(SEMAPHORE_ID, Message::DESTROY, _semaphore);
    }

    void p() {
        stub_syscall(SEMAPHORE_ID, Message::SYNCHRONIZER_LOCK, _semaphore);
    }

    void v() {
        stub_syscall(SEMAPHORE_ID, Message::SYNCHRONIZER_UNLOCK, _semaphore);
    }

private:
    void * _semaphore;
};

class Mutex_Stub : public Stub_Common
{
public:
    Mutex_Stub() {
        _mutex = reinterpret_cast<void *>(stub_syscall(MUTEX_ID, Message::Method_Id::CONSTRUCTOR1));
    }

    ~Mutex_Stub() {
        stub_syscall(MUTEX_ID, Message::Method_Id::DESTROY, _mutex);
    }

    void lock() {
        stub_syscall(MUTEX_ID, Message::Method_Id::SYNCHRONIZER_LOCK, _mutex);
    }
    void unlock() {
        stub_syscall(MUTEX_ID, Message::Method_Id::SYNCHRONIZER_UNLOCK, _mutex);
    }

private:
    void * _mutex;
};

#define Mutex Mutex_Stub
#define Semaphore Semaphore_Stub

#endif