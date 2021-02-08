// EPOS Scheduler Component Declarations

#ifndef __scheduler_h
#define __scheduler_h

#include <architecture/cpu.h>
#include <architecture/pmu.h>
#include <architecture/tsc.h>
#include <utility/scheduling.h>
#include <utility/math.h>
#include <utility/convert.h>

__BEGIN_SYS

// All scheduling criteria, or disciplines, must define operator int() with
// the semantics of returning the desired order of a given object within the
// scheduling list
class Scheduling_Criterion_Common
{
    friend class _SYS::Thread;
    friend class _SYS::Periodic_Thread;
    friend class _SYS::RT_Thread;
    friend class _SYS::Clerk<System>;         // for _statistics

public:
    // Priorities
    enum : int {
        ISR    = -1000,
            MAIN   = 0,
            HIGH   = 1,
            NORMAL = (unsigned(1) << (sizeof(int) * 8 - 1)) - 3,
            LOW    = (unsigned(1) << (sizeof(int) * 8 - 1)) - 2,
            IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) - 1
    };

    // Constructor helpers
    enum : unsigned int {
        SAME        = 0,
            NOW         = 0,
            UNKNOWN     = 0,
            ANY         = -1U
    };

    // Policy types
    enum : int {
        PERIODIC    = HIGH,
            APERIODIC   = NORMAL,
            SPORADIC    = NORMAL
    };

    // Policy traits
    static const bool timed = false;
    static const bool dynamic = false;
    static const bool preemptive = true;
    static const bool collecting = false;
    static const bool charging = false;
    static const bool awarding = false;
    static const bool migrating = false;
    static const bool track_idle = false;
    static const bool task_wide = false;
    static const bool cpu_wide = false;
    static const bool system_wide = false;
    static const unsigned int QUEUES = 1;

    // Runtime Statistics (for policies that don't use any; that´s why its a union)
    union Statistics {
        // Thread Execution Time
        TSC::Time_Stamp thread_execution_time;  // Sum of jobs execution time
        TSC::Time_Stamp last_thread_dispatch;   // The times tamp of the last dispatch
        // On context-switch: execution time += TSC::timestamp() - last_dispatch
        // Migration Auxiliary
        unsigned int destination_cpu;

        // ANN
        float input[COUNTOF(Traits<Monitor>::PMU_EVENTS)+COUNTOF(Traits<Monitor>::SYSTEM_EVENTS)-1];
        float output;

        // Deadline Miss count - Used By Clerk
        Alarm * alarm_times;            // Reference to RT_Thread private alarm (for monitoring pourposes)
        unsigned int finished_jobs;     // Number of finished jobs  <=> times alarm->p() has been called for this task
        unsigned int missed_deadlines;  // Number of finished jobs (finished_jobs) - number of dispatched jobs (alarm_times->times)

        // CPU Execution Time (capture ts)
        static TSC::Time_Stamp _cpu_time[Traits<Build>::CPUS];              // accumulated cpu time at each hyperperiod
        static TSC::Time_Stamp _last_dispatch_time[Traits<Build>::CPUS];         // Time Stamp of last dispatch
        static TSC::Time_Stamp _last_activation_time;                       // Global Time Stamp of the last heuristic activation
    };

protected:
    Scheduling_Criterion_Common() { _statistics.destination_cpu = ANY; }

public:
    const Microsecond period() { return 0;}
    void period(const Microsecond & p) {}

    unsigned int queue() const { return 0; }
    void queue(unsigned int q) {}

    bool update() { return false; }

    bool collect(bool end = false) { return false; }
    bool charge(bool end = false) { return true; }
    bool award(bool end = false) { return true; }

    volatile Statistics & statistics() { return _statistics; }

    static void init() {}

protected:
    Statistics _statistics;
};

// Priority (static and dynamic)
class Priority: public Scheduling_Criterion_Common
{
    friend class _SYS::Thread;
    friend class _SYS::Periodic_Thread;
    friend class _SYS::RT_Thread;

public:
    template <typename ... Tn>
    Priority(int p = NORMAL, Tn & ... an): _priority(p) {}

    operator const volatile int() const volatile { return _priority; }

protected:
    volatile int _priority;
};

// Round-Robin
class RR: public Priority
{
public:
    static const bool timed = true;
    static const bool dynamic = false;
    static const bool preemptive = true;

public:
    template <typename ... Tn>
    RR(int p = NORMAL, Tn & ... an): Priority(p) {}
};

// First-Come, First-Served (FIFO)
class FCFS: public Priority
{
public:
    static const bool timed = false;
    static const bool dynamic = false;
    static const bool preemptive = false;

public:
    template <typename ... Tn>
    FCFS(int p = NORMAL, Tn & ... an);
};


// Multicore Algorithms
class Variable_Queue_Scheduler
{
protected:
    Variable_Queue_Scheduler(unsigned int queue): _queue(queue) {};

    const volatile unsigned int & queue() const volatile { return _queue; }
    void queue(unsigned int q) { _queue = q; }

protected:
    volatile unsigned int _queue;
    static volatile unsigned int _next_queue;
};

// Global Round-Robin
class GRR: public RR
{
public:
    static const unsigned int HEADS = Traits<Machine>::CPUS;

public:
    template <typename ... Tn>
    GRR(int p = NORMAL, Tn & ... an): RR(p) {}

    static unsigned int current_head() { return CPU::id(); }
};

// Fixed CPU (fully partitioned)
class Fixed_CPU: public Priority, public Variable_Queue_Scheduler
{
public:
    static const bool timed = true;
    static const bool dynamic = false;
    static const bool preemptive = true;

    static const unsigned int QUEUES = Traits<Machine>::CPUS;

public:
    template <typename ... Tn>
    Fixed_CPU(int p = NORMAL, unsigned int cpu = ANY, Tn & ... an)
    : Priority(p), Variable_Queue_Scheduler(((_priority == IDLE) || (_priority == MAIN)) ? CPU::id() : (cpu != ANY) ? cpu : ++_next_queue %= CPU::cores()) {}

    using Variable_Queue_Scheduler::queue;
    static unsigned int current_queue() { return CPU::id(); }
};

// CPU Affinity
class CPU_Affinity: public Priority, public Variable_Queue_Scheduler
{
    friend class _SYS::Clerk<System>;         // for _statistics

public:
    static const bool timed = true;
    static const bool dynamic = false;
    static const bool preemptive = true;
    static const bool collecting = true;
    static const bool charging = true;
    static const bool awarding = true;
    static const bool heuristic = true;
    static const unsigned int QUEUES = Traits<Machine>::CPUS;

    struct Statistics {
        Statistics(): thread_execution_time(0), destination_cpu(ANY) {}

        TSC::Time_Stamp thread_execution_time;                              // accumulated execution time (i.e. sum of all jobs)
        unsigned int destination_cpu;                                       // for migrations

        Alarm * alarm_times;                                                // reference to Periodic_Thread->_alarm for monitoring purposes
        unsigned int finished_jobs;                                         // number of finished jobs (i.e. number of times Periodic_Thread->_alarm called Periodic_Thread->_semaphor->p() for this thread)
        unsigned int missed_deadlines;                                      // number of missed deadlines (finished_jobs - number of dispatched jobs (i.e. Periodic_Thread->_alarm->_times))

        static TSC::Time_Stamp _cpu_time[Traits<Build>::CPUS];              // accumulated cpu execution time in the current hyperperiod
        static TSC::Time_Stamp _last_dispatch_time[Traits<Build>::CPUS];    // time stamp of last dispatch
        static TSC::Time_Stamp _last_activation_time;                       // time stamp of the last heuristic activation

        static unsigned int _least_used_cpu;                                // cpu with lowest execution time
        static unsigned int _most_used_cpu;                                 // cpu with highest execution time
    };

public:
    template <typename ... Tn>
    CPU_Affinity(int p = NORMAL, unsigned int cpu = ANY, Tn & ... an)
    : Priority(p), Variable_Queue_Scheduler(((_priority == IDLE) || (_priority == MAIN)) ? CPU::id() : (cpu != ANY) ? cpu : ++_next_queue %= CPU::cores()) {}

    bool charge(bool end = false);
    bool award(bool end = false);

    volatile Statistics & statistics() { return _statistics; }

    using Variable_Queue_Scheduler::queue;
    static unsigned int current_queue() { return CPU::id(); }

private:
    Statistics _statistics;
};

__END_SYS

__BEGIN_UTIL

// Scheduling Queues
template<typename T>
class Scheduling_Queue<T, GRR>:
public Multihead_Scheduling_List<T> {};

template<typename T>
class Scheduling_Queue<T, Fixed_CPU>:
public Scheduling_Multilist<T> {};

__END_UTIL

#endif
