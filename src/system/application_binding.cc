// EPOS Application Binding

#include <utility/ostream.h>
#include <interface/message.h>
#include <stubs/process.h>

// Global objects
__BEGIN_SYS
OStream kerr;
__END_SYS

#include <stubs/ostream.h>

// Bindings
extern "C" {
    void _panic() { Thread::exit(-1); }
    void _exit(int s) { Thread::exit(s); for(;;); }
}

__USING_SYS;
extern "C" {
    void _syscall(void * m) { _syscall(m); }
    void _print(const char * s) { 
        OStream::print(s);
    }
    void _print_preamble() {}
    void _print_trailler(bool error) {}
}
