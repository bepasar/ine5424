// EPOS Thread Component Declarations

#ifndef __process_stub_h
#define __process_stub_h

#include <stubs/stub.h>
#include <stubs/memory.h>
#include <scheduler.h>

#define THREAD_ID EPOS::S::THREAD_ID
#define TASK_ID EPOS::S::TASK_ID

class Thread_Stub : public Stub_Common
{
public:
    friend class Task_Stub; // for private constructor           

protected:
    static const unsigned int STACK_SIZE = EPOS::S::Traits<EPOS::S::System>::multitask ? EPOS::S::Traits<EPOS::S::System>::STACK_SIZE : EPOS::S::Traits<EPOS::S::Application>::STACK_SIZE;
    static const unsigned int USER_STACK_SIZE = EPOS::S::Traits<EPOS::S::Application>::STACK_SIZE;

public:
    // Thread State
    enum State {
        RUNNING,
        READY,
        SUSPENDED,
        WAITING,
        FINISHING
    };

    // Thread Scheduling Criterion
    typedef EPOS::S::Traits<EPOS::S::Thread>::Criterion Criterion;
    enum {
        HIGH    = Criterion::HIGH,
        NORMAL  = Criterion::NORMAL,
        LOW     = Criterion::LOW,
        MAIN    = Criterion::MAIN,
        IDLE    = Criterion::IDLE
    };

    // Thread Configuration
    struct Configuration {
        Configuration(const State & s = READY, const Criterion & c = NORMAL, void * t = 0, unsigned int ss = STACK_SIZE, unsigned int uss = USER_STACK_SIZE)
        : state(s), criterion(c), task(t), stack_size(ss), user_stack_size(uss) {}

        State state;
        Criterion criterion;
        void * task;
        unsigned int stack_size;
        unsigned int user_stack_size;
    };

public:
    template<typename ... Tn>
    // constructor 1
    inline Thread_Stub(int (* entry)(Tn ...), Tn ... an) {
        _thread = reinterpret_cast<void *>(stub_syscall(THREAD_ID, Message::Method_Id::CONSTRUCTOR1, entry, an...));
    }

    template<typename ... Tn>
    // constructor 2
    Thread_Stub(const Configuration & conf, int (* entry)(Tn ...), Tn ... an) {
        _thread = reinterpret_cast<void *>(stub_syscall(THREAD_ID, Message::Method_Id::CONSTRUCTOR2, conf, entry, an...));
    }

    // destructor
    ~Thread_Stub() {
        stub_syscall(THREAD_ID, Message::Method_Id::DESTROY, 0, 1, 2);
    }

    const State state() { 
        return static_cast<const State>(stub_syscall(THREAD_ID, Message::Method_Id::THREAD_STATE, _thread));
    }

    const Criterion priority() const {
        return static_cast<const Criterion>(stub_syscall(THREAD_ID, Message::Method_Id::THREAD_PRIORITY_GET, _thread));
    }

    void priority(const Criterion & p) {
        stub_syscall(THREAD_ID, Message::Method_Id::THREAD_PRIORITY_SET, _thread, p);
    }

    void * task() const { 
        return reinterpret_cast<void *>(stub_syscall(THREAD_ID, Message::Method_Id::THREAD_TASK, _thread));
    }

    int join() {
        return stub_syscall(THREAD_ID, Message::Method_Id::THREAD_JOIN, _thread);
    }

    void pass() {
        stub_syscall(THREAD_ID, Message::Method_Id::THREAD_PASS, _thread);
    }

    void suspend() {
        stub_syscall(THREAD_ID, Message::Method_Id::THREAD_SUSPEND, _thread);
    }

    void resume() {
        stub_syscall(THREAD_ID, Message::Method_Id::THREAD_RESUME, _thread);
    }

    static Thread_Stub * volatile self() {
        void * thread = reinterpret_cast<void *>(stub_syscall(THREAD_ID, Message::Method_Id::SELF));
        return new Thread_Stub(thread);
    }

    static void yield() {
        stub_syscall(THREAD_ID, Message::Method_Id::THREAD_YIELD);
    }
    
    static void exit(int status = 0) {
        stub_syscall(THREAD_ID, Message::Method_Id::THREAD_EXIT, status);
    }

private:
    Thread_Stub(void * thread) : _thread(thread) { }

private:
    void * _thread;
};

class Task_Stub : public Stub_Common
{
    friend class Thread;        // for insert()

private:
    typedef EPOS::S::CPU::Log_Addr Log_Addr;
    typedef EPOS::S::CPU::Phy_Addr Phy_Addr;
    typedef EPOS::S::CPU::Context Context;

public:
    template<typename ... Tn>
    Task_Stub(Segment_Stub * code_segment, Segment_Stub * data_segment, int (* entry)(Tn ...), Tn ... an) {
        void * _code_segment = code_segment->_segment;
        void * _data_segment = data_segment->_segment;
        _task = reinterpret_cast<void *>(stub_syscall(TASK_ID, Message::Method_Id::CONSTRUCTOR1, _code_segment, _data_segment, entry, an ...));
    }

    template<typename ... Tn>
    Task_Stub(const Thread_Stub::Configuration & conf, Segment_Stub * code_segment, Segment_Stub * data_segment, int (* entry)(Tn ...), Tn ... an) {
        void * _code_segment = code_segment->_segment;
        void * _data_segment = data_segment->_segment;
        _task = reinterpret_cast<void *>(stub_syscall(TASK_ID, Message::Method_Id::CONSTRUCTOR2, conf, _code_segment, _data_segment, entry, an ...));
    }

    template<typename ... Tn>
    Task_Stub(int (* entry)(Tn ...), Tn ... an)
    {
        _task = reinterpret_cast<void *>(stub_syscall(TASK_ID, Message::Method_Id::CONSTRUCTOR3, entry, an ...));
    }

    ~Task_Stub() {
        stub_syscall(TASK_ID, Message::Method_Id::DESTROY, _task);
    }

    Address_Space_Stub * address_space() const { 
        void * address_space = reinterpret_cast<void *>(stub_syscall(TASK_ID, Message::Method_Id::TASK_ADDRESS_SPACE, _task)); 
        return new Address_Space_Stub(address_space);
    }

    Segment_Stub * code_segment() const { 
        void * segment = reinterpret_cast<void *>(stub_syscall(TASK_ID, Message::Method_Id::TASK_CODE_SEGMENT, _task)); 
        return new Segment_Stub(segment);
    }

    Segment_Stub * data_segment() const {
        void * segment = reinterpret_cast<void *>(stub_syscall(TASK_ID, Message::Method_Id::TASK_DATA_SEGMENT, _task)); 
        return new Segment_Stub(segment);
    }

    Log_Addr code() const {
        return static_cast<Log_Addr>(stub_syscall(TASK_ID, Message::Method_Id::TASK_CODE, _task));  
    }

    Log_Addr data() const {
        return static_cast<Log_Addr>(stub_syscall(TASK_ID, Message::Method_Id::TASK_DATA, _task)); 
    }

    Thread_Stub * main() const {
        void * thread = reinterpret_cast<void *>(stub_syscall(TASK_ID, Message::Method_Id::TASK_MAIN, _task));
        return new Thread_Stub(thread);
    }

    static Task_Stub * volatile self() {
        void * task = reinterpret_cast<void *>(stub_syscall(TASK_ID, Message::Method_Id::SELF)); 
        return new Task_Stub(task);
    }

private:
    Task_Stub(void * task) {
        _task = task;
    }
private:
    void * _task;
};

#define Thread Thread_Stub
#define Task Task_Stub

#endif
