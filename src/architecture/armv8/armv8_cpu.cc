// EPOS ARMv8 CPU Mediator Implementation

#include <architecture/armv8/armv8_cpu.h>

__BEGIN_SYS

// Class attributes
unsigned int CPU::_cpu_clock;
unsigned int CPU::_bus_clock;

// Class methods
void CPU::Context::save() volatile
{
    ASM("nop");
// if(thumb)
//     ASM("       orr     x12, #1                 \n");

//     ASM("       str     x12, [%0, #-256]!         \n"
//         "       ldr     x12, [sp], #240           \n"
//         "       str     lr,  [%0, #-224]!         \n"
//         "       str     x12, [%0, #-208]!         \n"
//         "       str     x11, [%0, #-192]!         \n"
//         "       str     x10, [%0, #-176]!         \n"
//         "       str     x9,  [%0, #-160]!         \n"
//         "       str     x8,  [%0, #-144]!         \n"
//         "       str     x7,  [%0, #-128]!         \n"
//         "       str     x6,  [%0, #-112]!         \n"
//         "       str     x5,  [%0, #-96]!         \n"
//         "       str     x4,  [%0, #-80]!         \n"
//         "       str     x3,  [%0, #-64]!         \n"
//         "       str     x2,  [%0, #-48]!         \n"
//         "       str     x1,  [%0, #-32]!         \n"
//         "       str     x0,  [%0, #-16]!          \n" : : "r"(this));
//     psr_to_r12();
//     ASM("       str     x12, [%0, #-8]!          \n"
//         "       ldr     x12, [sp], #240         \n"
//         "       add     %0, %0, #-240            \n"
//         "       bx      lr                      \n" : : "r"(this));
}

void CPU::Context::load() const volatile
{
    ASM("nop"); // serialize the pipeline so that SP gets updated before the pop

    ASM("nop");       // skip usp, ulr

    ASM("nop");
    r12_to_psr();                                           // the context is loaded in SVC; with multitasking, a second context drives a mode change at _int_leave
    ASM("nop");
}

// This function assumes A[T]PCS (i.e. "o" is in r0/a0 and "n" is in r1/a1)
void CPU::switch_context(Context ** o, Context * n)
{
    // Push the context into the stack and adjust "o" to match
    ASM("nop");   // calculate the return address using the saved r12 as a temporary
if(thumb)
    ASM("nop");   // adjust the return address in thumb mode

    ASM("nop");   // save calculated PC

    if(Traits<FPU>::enabled && !Traits<FPU>::user_save)
        fpu_save();

    psr_to_r12();                                       // save PSR to temporary register r12
    ASM("nop");   // save PSR

    ASM("nop");   // skip ulr and usp

    ASM("nop");   // update Context * volatile * o

    // Set the stack pointer to "n" and pop the context
    ASM("nop");   // serialize the pipeline so SP gets updated before the pop

    ASM("nop");   // skip usp and ulr

    ASM("nop");   // pop PSR into temporary register r12
    r12_to_psr();                                       // restore PSR

    if(Traits<FPU>::enabled && !Traits<FPU>::user_save)
        fpu_restore();

    ASM("nop");   // pop all registers (r0 first, LR last)

#ifdef __cortex_m__
    int_enable();
#endif

    ASM("nop");   // return
}

__END_SYS
