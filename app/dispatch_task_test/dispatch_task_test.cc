/* Dispatch Task Test
 *
 * For this test we use a non timed Round-Robin. Thus, threads will be preempted
 * upon creation. Because main threads have higher priority, they should sleep to
 * allow normal threads to run. The preemption of threads verifies the correctness
 * of dispatch context switch.
 */

#include <utility/ostream.h>
#include <stubs/process.h>
#include <stubs/time.h>
#include <stubs/synchronizer.h>

#define TASKS 3
#define THREADS 3
#define DELAY 100000

using namespace EPOS::S;

OStream cout;

int id;

/*
 * Print task and its current thread id. It also compares the expected task id with the 
 * actual task id.
 */
int say_hello(int expected_id, int thread_id) {
    cout << "thread number " << id << "." << thread_id << "!!" << endl;
    assert(id == expected_id);
    return 0;
}

/*
 * Initialize threads and allow them to run.
 */
void _test_dispatch(int expected_id) {
    Thread * thread;
    for (int i = 0; i < THREADS; i++) {
        thread = new Thread(&say_hello, expected_id, i);
        // Allow thread to run
        Alarm::delay(DELAY);
        cout << id << "." << i << ": thread address = " << thread << endl;
        delete thread;
    }
}

/*
 * Calls _test_dispatch. Function call verifies that lr register is not lost
 * on context switch.
 */
int test_dispatch(int expected_id) {
    _test_dispatch(expected_id);
    cout << id << ": context switch ok!" << endl;
    return 0;
}

int main() {
    Chronometer chronometer;
    chronometer.start();

    cout << "main task adress =  " << Task::self() << endl;
    cout << "----------------------------------------------------" << endl;
    Task * task;
    for (int i = 1; i <= TASKS; i++) {
        id = i;
        task = new Task(&test_dispatch, id);
        // Allow threads to run
        Alarm::delay(DELAY * (THREADS + 1));
        cout << "----------------------------------------------------" << endl;
        cout << i << ": task address = " << task << endl;
        cout << "----------------------------------------------------" << endl;
        delete task;
    }
    cout << "Bye world, I am the main Task!" << endl;

    chronometer.stop();
    Microsecond time = chronometer.read();
    cout << "test took " << time << " microseconds long" << endl;
    return 0;
}