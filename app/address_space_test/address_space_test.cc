// EPOS Address Space Test Program

#include <stubs/memory.h>
#include <utility/ostream.h>
#include <stubs/process.h>
#include <machine.h>
#include <stubs/time.h>
#include <stubs/synchronizer.h>
#include <architecture/cpu.h>

#define DELAY 10000
#define TASKS 4

using namespace EPOS::S;

OStream cout;

int id;

int assert_changed_addess_space(int previous) {
    int current = *(reinterpret_cast<unsigned long*>(Task::self()->address_space()));
    cout << "current_address_space=" << current << endl;
    cout << "previous_address_space=" << previous << endl;
    assert(current != previous);
    Alarm::delay(DELAY);
    assert(current != previous);
    return 0;
}

int main()
{
    Chronometer chronometer;
    chronometer.start();

    Task * task[TASKS];
    for (int i = 1; i <= TASKS; i++) {
        id = i-1;
        int current = *(reinterpret_cast<unsigned long*>(Task::self()->address_space()));
        task[i-1] = new Task(&assert_changed_addess_space, current);
    }

    Alarm::delay(DELAY);

    for (int i = 0; i < TASKS; i++) {
        delete task[i];
    }

    chronometer.stop();
    Microsecond time = chronometer.read();
    cout << "test took " << time << " microseconds long" << endl;
    return 0;
}
