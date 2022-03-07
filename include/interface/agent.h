// EPOS Component Framework - Component Agent

#ifndef __agent_h
#define __agent_h

#include <process.h>
#include <memory.h>
#include <system.h>
#include <time.h>
#include <synchronizer.h>

#include <machine/display.h>
#include <architecture/cpu.h>
#include <interface/message.h>

#define HANDLERS 12

__BEGIN_SYS

class Agent: public Message
{
private:
    typedef void (Agent:: * Member)();

public:
    void exec() {
        unsigned int handler = id();
        if (handler <= HANDLERS) {
            (this->*_handlers[handler])();
        } else {
            Machine::panic();
        }
    }

public:
    static void software_interrupt(void * param);
private:
    void handle_thread();
    void handle_task();
    void handle_active();
    void handle_address_space();
    void handle_segment();
    void handle_mutex();
    void handle_semaphore();
    void handle_condition();
    void handle_clock();
    void handle_alarm();
    void handle_chronometer();
    void handle_utility();

public:
    static Member _handlers[HANDLERS];
};

void Agent::software_interrupt(void * param) {
    Agent * message = reinterpret_cast<Agent *>(param);
    message->exec(); // syscall
}

Agent::Member Agent::_handlers[HANDLERS] =
    {
        &Agent::handle_thread,
        &Agent::handle_task,
        &Agent::handle_active,
        &Agent::handle_address_space,
        &Agent::handle_segment,
        &Agent::handle_mutex,
        &Agent::handle_semaphore,
        0,
        0,
        &Agent::handle_alarm,
        &Agent::handle_chronometer,
        &Agent::handle_utility,
    };

void Agent::handle_thread()
{
    Thread * thread;
    Thread::Configuration configuration;
    int (* entry)(CPU::Reg, CPU::Reg, CPU::Reg, CPU::Reg);
    CPU::Reg x0, x1, x2, x3;
    Thread::Criterion p;

    Result res = 0;

    switch(method()) {
        case CONSTRUCTOR1:
            out(&entry, &x0, &x1, &x2, &x3);
            res = reinterpret_cast<Result>(new (SYSTEM) Thread(entry, x0, x1, x2 ,x3));
            break;
        case CONSTRUCTOR2:
            out(&configuration, &entry, &x0, &x1, &x2, &x3);
            res = reinterpret_cast<Result>(new (SYSTEM) Thread(configuration, entry, x0, x1, x2, x3));
            break;
        case DESTROY:
            out(&thread);
            delete thread;
            break;
        case THREAD_STATE:
            out(&thread);
            res = static_cast<Result>(thread->state());
            break;
        case THREAD_PRIORITY_GET:
            out(&thread);
            res = static_cast<Result>(thread->priority());
            break;
        case THREAD_PRIORITY_SET:
            out(&thread, &p);
            thread->priority(p);
            break;
        case THREAD_TASK:
            out(&thread);
            res = reinterpret_cast<Result>(thread->task());
            break;
        case THREAD_JOIN:
            out(&thread);
            res = thread->join();
            break;
        case THREAD_PASS:
            out(&thread);
            thread->pass();
            break;
        case THREAD_SUSPEND:
            out(&thread);
            thread->suspend();
            break;
        case THREAD_RESUME:
            out(&thread);
            thread->resume();
            break;
        case SELF:
            res = reinterpret_cast<Result>(Thread::self());
            break;
        case THREAD_YIELD:
            Thread::yield();
            break;
        case THREAD_EXIT:
            int status;
            out(&status);
            Thread::exit(status);
            break;
        default:
            res = UNDEFINED;
    }

    result(res);
};

void Agent::handle_active() {}

void Agent::handle_address_space() {
    Result res = 0;
    Address_Space * address_space;
    Segment * segment;
    CPU::Log_Addr addr;

    switch(method()) {
        case CONSTRUCTOR1:
            res = reinterpret_cast<Result>(new (SYSTEM) Address_Space());
        case CONSTRUCTOR2:
            MMU::Page_Directory * pd;
            out(&pd);
            res = reinterpret_cast<Result>(new (SYSTEM) Address_Space(pd));
            break;
        case DESTROY:
            out(&address_space);
            delete address_space;
            break;
        case ADDRESS_SPACE_ATTACH1:
            out(&address_space, &segment);
            res = static_cast<Result>(address_space->attach(segment));
            break;
        case ADDRESS_SPACE_ATTACH2:
            unsigned int from;
            out(&address_space, &segment, &from);
            res = static_cast<Result>(address_space->attach(segment, from));
            break;
        case ADDRESS_SPACE_ATTACH3:
            out(&address_space, &segment, &addr);
            res = static_cast<Result>(address_space->attach(segment, addr));
            break;
        case ADDRESS_SPACE_DETACH1:
            out(&address_space, &segment);
            address_space->detach(segment);
            break;
        case ADDRESS_SPACE_DETACH2:
            out(&address_space, &segment, &addr);
            address_space->detach(segment, addr);
            break;
        case ADDRESS_SPACE_PHYSICAL:
            out(&address_space, &addr);
            res = static_cast<Result>(address_space->physical(addr));
            break;
        default :
            res = UNDEFINED;
    }
        
    result(res);
}

void Agent::handle_segment() {
    Segment * segment;
    Segment::Flags flags;
    Segment::Phy_Addr phy_addr;
    int amount;
    unsigned int bytes;
    unsigned int to;
    unsigned int from;

    Result res = 0;

    switch(method()) {
        case CONSTRUCTOR1:
            out(&bytes, &flags);
            res = reinterpret_cast<Result>(new (SYSTEM) Segment(bytes, flags));
            break;
        case CONSTRUCTOR2:
            out(&phy_addr, &bytes, &flags);
            res = reinterpret_cast<Result>(new (SYSTEM) Segment(phy_addr, bytes, flags));
            break;
        case CONSTRUCTOR3:
            out(&phy_addr, &from, &to, &flags);
            res = reinterpret_cast<Result>(new (SYSTEM) Segment(phy_addr, from, to, flags));
            break;
        case DESTROY:
            out(&segment);
            delete segment;
            break;
        case SEGMENT_SIZE:
            out(&segment);
            res = static_cast<Result>(segment->size());
            break;
        case SEGMENT_PHY_ADDRESS:
            out(&segment);
            res = static_cast<Result>(segment->phy_address());
            break;
        case SEGMENT_RESIZE:
            out(&segment, &amount);
            res = static_cast<Result>(segment->resize(amount));
            break;
        default:
            res = UNDEFINED;
    }

    result(res);
}

void Agent::handle_task()
{
    Task * task;
    Thread::Configuration configuration;
    int (* entry)(CPU::Reg, CPU::Reg, CPU::Reg, CPU::Reg);
    Segment * data;
    Segment * code;
    CPU::Reg x0, x1, x2, x3;

    Result res = 0;

    switch(method()) {
        case CONSTRUCTOR1:
            out(&code, &data, &entry, &x0, &x1, &x2, &x3);
            res = reinterpret_cast<Result>(new (SYSTEM) Task(code, data, entry, x0, x1, x2 ,x3));
            break;
        case CONSTRUCTOR2:
            out(&configuration, &code, &data, &entry, &x0, &x1, &x2, &x3);
            res = reinterpret_cast<Result>(new (SYSTEM) Task(configuration, code, data, entry, x0, x1, x2, x3));
            break;
        case CONSTRUCTOR3:
            out(&entry, &x0, &x1, &x2, &x3);
            res = reinterpret_cast<Result>(new (SYSTEM) Task(entry, x0, x1, x2, x3));
            break;
        case DESTROY:
            out(&task);
            delete task;
            break;
        case TASK_ADDRESS_SPACE:
            out(&task);
            res = reinterpret_cast<Result>(task->address_space());
            break;
        case TASK_CODE_SEGMENT:
            out(&task);
            res = static_cast<Result>(task->code());
            break;
        case TASK_DATA_SEGMENT:
            out(&task);
            res = static_cast<Result>(task->data());
            break;
        case TASK_CODE:
            out(&task);
            res = static_cast<Result>(task->code());
            break;
        case TASK_DATA:
            out(&task);
            res = static_cast<Result>(task->data());
            break;
        case TASK_MAIN:
            out(&task);
            res = reinterpret_cast<Result>(task->main());
            break;
        case SELF:
            res = reinterpret_cast<Result>(Task::self());
            break;
        default:
            res = UNDEFINED;
    }

    result(res);
};

void Agent::handle_mutex()
{
    Mutex * mutex;
    Result res = 0;

    switch(method()) {
        case CONSTRUCTOR1:
            res = reinterpret_cast<Result>(new (SYSTEM) Mutex());
            break;
        case DESTROY:
            out(&mutex);
            delete mutex;
            break;
        case SYNCHRONIZER_LOCK:
            out(&mutex);
            mutex->lock();
            break;
        case SYNCHRONIZER_UNLOCK:
            out(&mutex);
            mutex->unlock();
            break;
        default:
            res = UNDEFINED;
    }

    result(res);
};

void Agent::handle_semaphore() {
    Result res = 0;
    Semaphore * semaphore;

    switch(method()) {
        case CONSTRUCTOR1:
            int v;
            out(&v);
            res = reinterpret_cast<Result>(new (SYSTEM) Semaphore(v));
            break;
        case DESTROY:
            out(&semaphore);
            delete semaphore;
            break;
        case SYNCHRONIZER_LOCK:
            out(&semaphore);
            semaphore->p();
            break;
        case SYNCHRONIZER_UNLOCK:
            out(&semaphore);
            semaphore->v();
            break;
        default:
            res = UNDEFINED;
    }

    result(res);
}

void Agent::handle_alarm()
{
    Alarm * alarm;
    Microsecond time;
    Handler * handler;
    unsigned int times;
    Result res = 0;

    switch(method()) {
        case CONSTRUCTOR1:
            out(&time, &handler, &times);
            res = reinterpret_cast<Result>(new (SYSTEM) Alarm(time, handler, times));
            break;
        case DESTROY:
            out(&alarm);
            delete alarm;
            break;
        case ALARM_GET_PERIOD:
            out(&alarm);
            res = static_cast<Result>(alarm->period());
            break;
        case ALARM_SET_PERIOD:
            out(&alarm, &time);
            alarm->period(time);
            break;
        case RESET:
            out(&alarm);
            alarm->reset();
            break;
        case FREQUENCY:
            res = static_cast<Result>(Alarm::frequency());
            break;
        case ALARM_DELAY:
            out(&time);
            Alarm::delay(time);
            break;
        default:
            res = UNDEFINED;
    }
    result(res);
};

void Agent::handle_chronometer() {
    Result res = 0;
    Chronometer * chronometer;

    switch(method()) {
        case CONSTRUCTOR1:
            res = reinterpret_cast<Result>(new (SYSTEM) Chronometer());
            break;
        case DESTROY:
            out(&chronometer);
            delete chronometer;
            break;
        case FREQUENCY:
            out(&chronometer);
            res = static_cast<Result>(chronometer->frequency());
            break;
        case RESET:
            out(&chronometer);
            chronometer->frequency();
            break;
        case CHRONOMETER_START:
            out(&chronometer);
            chronometer->start();
            break;
        case CHRONOMETER_LAP:
            out(&chronometer);
            chronometer->lap();
            break;
        case CHRONOMETER_STOP:
            out(&chronometer);
            chronometer->stop();
            break;
        case CHRONOMETER_READ:
            out(&chronometer);
            res = static_cast<Result>(chronometer->read());
            break;
        default:
            res = UNDEFINED;
    }

    result(res);
}

void Agent::handle_utility() {
    switch(method()) {
        case OS_STREAM_PRINT:
            const char * s;
            out(&s);
            _print(s);
            break;
    }
}



__END_SYS

#endif
