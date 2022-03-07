#include <utility/ostream.h>
#include <time.h>
#include <real-time.h>

using namespace EPOS;

OStream cout;

Chronometer chronometer;

Periodic_Thread * thread_a;
Periodic_Thread * thread_b;
Periodic_Thread * thread_c; 

int i = 0;
char * scheduling;

void meaningless_work()
{
    int end = 10000;

    for (int i = 0; i < end; ++i)
    {
        for (int j = 0; j < end; ++j)
        {
            for (int k = 0; k < end; ++k)
            {
            }
        }
    }
}

int work(char thread_id)
{
    Microsecond t;
    int jobs = 1; // jobs counter

    do
    {
        t = chronometer.read(); // start_time

        cout << i << ": thread_" << thread_id << " - job " << jobs << " -> start_time:  " << t << endl;
        scheduling[i++] = thread_id;

        meaningless_work();

        t = chronometer.read(); // finish_time
        cout << i << ": thread_" << thread_id << " - job " << jobs++ << " -> finish_time: " << t << endl;

    } while (Periodic_Thread::wait_next());

    return (int)t;
}

void RM_scheduling_test()
{
    const int period_a = 100000; // 100 ms
    const int period_b = 150000; // 150 ms
    const int period_c = 350000; // 350 ms

    const int jobs_a = 10;
    const int jobs_b = 7;
    const int jobs_c = 4;

    scheduling = new char[jobs_a + jobs_b + jobs_c];
    char expected_scheduling[jobs_a + jobs_b + jobs_c] = {'A','B','C','A','B','A','A','B','C','A','B','A','A','B','A','C','B','A','A','B','C'};

    thread_a = new Periodic_Thread(RTConf(period_a, period_a, 0, 0, jobs_a), &work, 'A');
    thread_b = new Periodic_Thread(RTConf(period_b, period_b, 0, 0, jobs_b), &work, 'B');
    thread_c = new Periodic_Thread(RTConf(period_c, period_c, 0, 0, jobs_c), &work, 'C');

    cout << endl
         << "Thread priorities:" << endl
         << "thread_A: " << thread_a->priority() << endl
         << "thread_B: " << thread_b->priority() << endl
         << "thread_C: " << thread_c->priority() << endl
         << endl;

    cout << "Jobs timestamps:" << endl;
    chronometer.start();

    thread_a->join();
    thread_b->join();
    thread_c->join();

    chronometer.stop();

    cout << endl
         << "Scheduling order:" << endl;
    for (int n = 0; n < (jobs_a + jobs_b + jobs_c); n++) {
        assert(scheduling[n] == expected_scheduling[n]);
        cout << scheduling[n] << " -> ";
    }
}

int main()
{
    RM_scheduling_test();
    return 0;
}