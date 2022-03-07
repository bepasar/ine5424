/* Interrupt Task Test
 *
 * For this test we use a timed, non-preemptive Round-Robin. Thus, threads will
 * only be preempted at each timer interrupt.
 * 
 * Tasks created by main will create new threads and test whether the value
 * 'expected_id' given to them by the main Task is correct (i.e. the address
 * space is working properly).
 */

#include <utility/ostream.h>
#include <stubs/process.h>
#include <machine.h>
#include <stubs/time.h>
#include <stubs/synchronizer.h>
#include <architecture/cpu.h>

#define TASKS 1
#define THREADS 4
#define LOOP_SIZE 10
#define DELAY 100000

using namespace EPOS::S;

OStream cout;

int id;

/*
 * Print task and its current thread id. It also compares the expected task id with the 
 * actual task id.
 */
void say_hello(int expected_id, int thread_id) {
    cout << "Hello world, I am the Thread number " << id << "." << thread_id << "!!" << endl;
    assert(id == expected_id);
    cout << "Bye world, I am the Thread number " << id << "." << thread_id << "!!" << endl;
}

/*
 * Halt the current task's thread by n iterations. Call the say_hello method to check the ids.
 * Delay the current task's thread by a fixed amount.
 */
int halt(int expected_id, int thread_id) {
    int n = LOOP_SIZE;
    while (n--) {
        say_hello(expected_id, thread_id);
        Alarm::delay(DELAY);
    }
    return 0;
}

/*
 * Creates new threads and allow them to run and print their names.
 */
void _test_interrupt_dispatch(int expected_id) {
    Thread * thread[THREADS];
    cout << "----------------------------------------------------" << endl;
    for (int i = 0; i < THREADS; i++) {
        thread[i] = new Thread(&halt, expected_id, i);
        cout << id << "." << i << ": thread address = " << thread[i] << endl;
    }
    cout << "----------------------------------------------------" << endl;
    // allow threads to run
    Alarm::delay(DELAY * (LOOP_SIZE * THREADS + 1));
    for (int i = 0; i < THREADS; i++) {
        delete thread[i];
    }
}

/*
 * Entry point for new tasks. Checks if 'lr' register is saved correctly
 * during context switches.
 */
int test_interrupt_dispatch(int expected_id) {
    _test_interrupt_dispatch(expected_id);
    cout << id << ": register lr is correctly saved on context switch" << endl;
    return 0;
}

int main() {
    Chronometer chronometer;
    chronometer.start();

    cout << "Hello world, I am the main Task!" << endl;
    cout << "----------------------------------------------------" << endl;
    Task * task[TASKS];
    for (int i = 1; i <= TASKS; i++) {
        id = i;
        task[i-1] = new Task(&test_interrupt_dispatch, i);
        Alarm::delay(DELAY * (THREADS + 1));
        cout << id << ": task address = " << task[i-1] << endl;
    }
    cout << "----------------------------------------------------" << endl;
    // changing id should not affect other tasks
    id = 0;
    // allow threads to run
    Alarm::delay((TASKS + 1) * DELAY * (LOOP_SIZE + 1));
    assert(id == 0);
    cout << "----------------------------------------------------" << endl;
    cout << "id after execution: " << id << endl;
    cout << "----------------------------------------------------" << endl;
    for (int i = 0; i < TASKS; i++) {
        delete task[i];
    }
    cout << "Bye world, I am the main Task!" << endl;

    chronometer.stop();
    Microsecond time = chronometer.read();
    cout << "test took " << time << " microseconds long" << endl;
    return 0;
}
