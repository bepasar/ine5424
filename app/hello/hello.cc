#include <utility/ostream.h>
#include <stubs/process.h>

using namespace EPOS::S::U;

OStream cout;

int say_hello(int id0, int id1, int id2, int id3) {
    cout << "Hello, i am thread " << id0 << ", " << id1 << ", " << id2 << ", " << id3 << endl;
    return 0;
}

int main()
{
    cout << "Hello world! I am main thread" << endl;
    Thread * thread = new Thread(&say_hello, 1, 2, 3, 4);
    cout << "created thread " << thread << endl;
    thread->join();
    cout << "Bye world! I am main thread" << endl;
    return 0;
}
