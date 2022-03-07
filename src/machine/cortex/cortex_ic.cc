// EPOS ARM Cortex-A IC Mediator Implementation

#include <machine/machine.h>
#include <machine/ic.h>
#include <machine/timer.h>
#include <process.h>
#include <interface/agent.h>

#ifdef __cortex_a__

extern "C" { void _int_entry() __attribute__ ((naked, nothrow, alias("_ZN4EPOS1S2IC5entryEv"))); }
extern "C" { void _eoi(unsigned int) __attribute__ ((alias("_ZN4EPOS1S2IC3eoiEj"))); }
extern "C" { void __exit(); }

__BEGIN_SYS

IC::Interrupt_Handler IC::_int_vector[IC::INTS];

void IC::dispatch(unsigned int i)
{
#ifdef __arch_armv8__
    i = int_id();
#endif

    if((i != INT_SYS_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::dispatch(i=" << i << ")" << endl;

    assert(i < INTS);
    if(_eoi_vector[i])
        _eoi_vector[i](i);

    CPU::int_enable();  // Cortex-A disable interrupts at each interrupt

    _int_vector[i](i);
}

void IC::eoi(unsigned int id)
{
    if((id != INT_SYS_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::eoi(i=" << id << ")" << endl;

    assert(id < INTS);
    if(_eoi_vector[id])
        _eoi_vector[id](id);
}

void IC::int_not(Interrupt_Id i)
{
    db<IC>(WRN) << "IC::int_not(i=" << i << ")" << endl;
}

#ifdef __arch_armv7__

extern "C" { void _reserved() __attribute__ ((alias("_ZN4EPOS1S2IC8reservedEv"))); }
extern "C" { void _undefined_instruction() __attribute__ ((alias("_ZN4EPOS1S2IC21undefined_instructionEv"))); }
extern "C" { void _software_interrupt() __attribute__ ((alias("_ZN4EPOS1S2IC18software_interruptEv"))); }
extern "C" { void _prefetch_abort() __attribute__ ((alias("_ZN4EPOS1S2IC14prefetch_abortEv"))); }
extern "C" { void _data_abort() __attribute__ ((alias("_ZN4EPOS1S2IC10data_abortEv"))); }
extern "C" { void _fiq() __attribute__ ((alias("_ZN4EPOS1S2IC3fiqEv"))); }

void IC::entry()
{
    // We assume A[T]PCS ARM ABI, so any function using registers r4 until r11 will save those upon beginning and restore them when exiting.
    // An interrupt can happen in the middle of one such function, but if the ISR drives the PC through other functions that use the same registers, they will save and restore them. We therefore don't need to save r4-r11 here.
    CPU::svc_enter(CPU::MODE_IRQ);
    dispatch(int_id());
    CPU::svc_leave();
}

void IC::prefetch_abort()
{
    // A prefetch abort on __exit is triggered by MAIN after returning to CRT0 and by the other threads after returning using the LR initialized at creation time to invoke Thread::exit()

    CPU::svc_enter(CPU::MODE_ABORT, false); // enter SVC to capture LR (the faulting address) in r1
    if((CPU::r1()) != CPU::Reg(&__exit))
        db<IC, Machine>(TRC) << "IC::prefetch_abort() [addr=" << CPU::r1() << "]" << endl;
    CPU::svc_stay();  // undo the context saving of svc_enter(), but do not leave SVC
    kill();
}

void IC::undefined_instruction()
{
    CPU::svc_enter(CPU::MODE_UNDEFINED, false); // enter SVC to capture LR (the faulting address) in r1
    db<IC>(TRC) << "IC::undefined_instruction(addr=" << CPU::r1() << ")" << endl;
    CPU::svc_stay();  // undo the context saving of svc_enter(), but do not leave SVC
    kill();
}

void IC::software_interrupt()
{
    CPU::svc_enter(CPU::MODE_UNDEFINED, false); // enter SVC to capture LR (the faulting address) in r1
    db<IC>(TRC) << "IC::software_interrupt(addr=" << CPU::r1() << ")" << endl;
    CPU::svc_stay();  // undo the context saving of svc_enter(), but do not leave SVC
    kill();
}

void IC::data_abort()
{
    CPU::svc_enter(CPU::MODE_ABORT, false); // enter SVC to capture LR (the faulting address) in r1
    db<IC, Machine>(TRC) << "IC::data_abort(addr=" << CPU::r1() << ")" << endl;
    CPU::svc_stay();  // undo the context saving of svc_enter(), but do not leave SVC
    kill();
}

void IC::reserved()
{
    CPU::svc_enter(CPU::MODE_ABORT, false); // enter SVC to capture LR (the faulting address) in r1
    db<IC, Machine>(WRN) << "IC::reserved/reset(addr=" << CPU::r1() << ")" << endl;
    CPU::svc_stay();  // undo the context saving of svc_enter(), but do not leave SVC
    Machine::panic();
}

void IC::fiq()
{
    CPU::svc_enter(CPU::MODE_FIQ, false); // enter SVC to capture LR (the return address) in r1
    CPU::svc_stay();  // undo the context saving of svc_enter(), but do not leave SVC
    kill();
}

void IC::kill()
{
    db<IC>(WRN) << "The running thread will now be terminated!" << endl;
    Thread::exit(-1);
}

#else

extern "C" { void _dispatch(unsigned int) __attribute__ ((alias("_ZN4EPOS1S2IC8dispatchEj"))); }
extern "C" { void irq() __attribute__ ((nothrow));}
extern "C" { void software_interrupt() __attribute__ ((nothrow));}

void IC::entry()
{
	Machine::panic();
}

void IC::irq()
{
    ASM("   stp     x29, x30, [sp,#-16]!        \t\n\
            stp     x27, x28, [sp,#-16]!        \t\n\
            stp     x25, x26, [sp,#-16]!        \t\n\
            stp     x23, x24, [sp,#-16]!        \t\n\
            stp     x21, x22, [sp,#-16]!        \t\n\
            stp     x19, x20, [sp,#-16]!        \t\n\
            stp     x17, x18, [sp,#-16]!        \t\n\
            stp     x15, x16, [sp,#-16]!        \t\n\
            stp     x13, x14, [sp,#-16]!        \t\n\
            stp     x11, x12, [sp,#-16]!        \t\n\
            stp     x9, x10, [sp,#-16]!         \t\n\
            stp     x7, x8, [sp,#-16]!          \t\n\
            stp     x5, x6, [sp,#-16]!          \t\n\
            stp     x3, x4, [sp,#-16]!          \t\n\
            stp     x1, x2, [sp,#-16]!          \t\n\
            str     x0, [sp,#-8]!               \t\n\
            mrs     x30, spsr_el1               \t\n\
            mrs     x29, elr_el1                \t\n\
            stp     x29, x30, [sp,#-16]!        \t\n\
            bl      _dispatch                   \t\n\
            ldp     x29, x30, [sp], #16         \t\n\
            msr     elr_el1, x29                \t\n\
            msr     spsr_el1, x30               \t\n\
            ldr     x0, [sp], #8                \t\n\
            ldp     x1, x2, [sp], #16           \t\n\
            ldp     x3, x4, [sp], #16           \t\n\
            ldp     x5, x6, [sp], #16           \t\n\
            ldp     x7, x8, [sp], #16           \t\n\
            ldp     x9, x10, [sp], #16          \t\n\
            ldp     x11, x12, [sp], #16         \t\n\
            ldp     x13, x14, [sp], #16         \t\n\
            ldp     x15, x16, [sp], #16         \t\n\
            ldp     x17, x18, [sp], #16         \t\n\
            ldp     x19, x20, [sp], #16         \t\n\
            ldp     x21, x22, [sp], #16         \t\n\
            ldp     x23, x24, [sp], #16         \t\n\
            ldp     x25, x26, [sp], #16         \t\n\
            ldp     x27, x28, [sp], #16         \t\n\
            ldp     x29, x30, [sp], #16         \t");
}

void IC::software_interrupt()
{
    // primeiro testa se exception class é de SVC
    ASM("   stp     x29, x30, [sp,#-16]!        \t\n\
            mrs     x29, esr_el1                \t\n\
            lsr     x30, x29, #26               \t\n\
            cmp     x30, #21                    \t\n\
            b.ne    _int_entry                  \t\n\
                                                \t\n\
            stp     x27, x28, [sp,#-16]!        \t\n\
            stp     x25, x26, [sp,#-16]!        \t\n\
            stp     x23, x24, [sp,#-16]!        \t\n\
            stp     x21, x22, [sp,#-16]!        \t\n\
            stp     x19, x20, [sp,#-16]!        \t\n\
            stp     x17, x18, [sp,#-16]!        \t\n\
            stp     x15, x16, [sp,#-16]!        \t\n\
            stp     x13, x14, [sp,#-16]!        \t\n\
            stp     x11, x12, [sp,#-16]!        \t\n\
            stp     x9, x10, [sp,#-16]!         \t\n\
            stp     x7, x8, [sp,#-16]!          \t\n\
            stp     x5, x6, [sp,#-16]!          \t\n\
            stp     x3, x4, [sp,#-16]!          \t\n\
            stp     x1, x2, [sp,#-16]!          \t\n\
            str     x0, [sp,#-8]!               \t\n\
            mrs     x30, spsr_el1               \t\n\
            mrs     x29, elr_el1                \t\n\
            stp     x29, x30, [sp,#-16]!        \t");

    void * message = reinterpret_cast<void *>(CPU::r0());
    Agent::software_interrupt(message);

    ASM("   ldp     x29, x30, [sp], #16         \t\n\
            msr     elr_el1, x29                \t\n\
            msr     spsr_el1, x30               \t\n\
            ldr     x0, [sp], #8                \t\n\
            ldp     x1, x2, [sp], #16           \t\n\
            ldp     x3, x4, [sp], #16           \t\n\
            ldp     x5, x6, [sp], #16           \t\n\
            ldp     x7, x8, [sp], #16           \t\n\
            ldp     x9, x10, [sp], #16          \t\n\
            ldp     x11, x12, [sp], #16         \t\n\
            ldp     x13, x14, [sp], #16         \t\n\
            ldp     x15, x16, [sp], #16         \t\n\
            ldp     x17, x18, [sp], #16         \t\n\
            ldp     x19, x20, [sp], #16         \t\n\
            ldp     x21, x22, [sp], #16         \t\n\
            ldp     x23, x24, [sp], #16         \t\n\
            ldp     x25, x26, [sp], #16         \t\n\
            ldp     x27, x28, [sp], #16         \t\n\
            ldp     x29, x30, [sp], #16         \t");
}


#endif    

__END_SYS

#endif
