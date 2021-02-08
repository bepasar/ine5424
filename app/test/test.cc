#include <utility/ostream.h>
#include <machine.h>
#include <time.h>
#include <synchronizer.h>
#include <process.h>

using namespace EPOS;

OStream cout;

unsigned int io_iterations = 10;
unsigned int math_iterations = 20;

int io_job() {
    for (unsigned int i = 0; i < io_iterations; i++) {
        Alarm::delay(100000);
        cout << "IO[" << CPU::id() << "]\n" << endl;
    }
    return 0;
}

int math_job() {
    unsigned int tester = 0;
    for (unsigned int i = 0; i < math_iterations; i++) {
        for(int k = 0; k <= 1000000; k++)
            tester += k;
        cout << "FOR[" << CPU::id() << "]\n" << endl;
        Alarm::delay(10000);
    }
    return tester;
}

int main()
{
    Thread* io0 = new Thread(&io_job);
    Thread* math_job0 = new Thread(&math_job);
    Thread* math_job1 = new Thread(&math_job);
    Thread* math_job2 = new Thread(&math_job);
    cout << "Hello world!" << endl;
    for (unsigned int i = 0; i < math_iterations; i++) {
        for(int k = 0; k <= 1000000; k++);
        cout << "FOR[" << CPU::id() << "]\n" << endl;
        Alarm::delay(10000);
    }
    io0->join();
    math_job0->join();
    math_job1->join();
    math_job2->join();
    cout << "Goodbye world!" << endl;
    return 0;
}
