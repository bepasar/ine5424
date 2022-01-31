// EPOS ARMv8 CPU Mediator Declarations

#ifndef __armv8_h
#define __armv8_h

#include <architecture/cpu.h>

__BEGIN_SYS

class ARMv8: protected CPU_Common
{
    friend class Init_System; // for CPU::init()

protected:
    static const bool multicore = Traits<System>::multicore;
    static const bool multitask = Traits<System>::multitask;

public:
    // CPU Native Data Types
    using CPU_Common::Reg8;
    using CPU_Common::Reg16;
    using CPU_Common::Reg32;
    using CPU_Common::Reg64;
    using CPU_Common::Reg;
    using CPU_Common::Log_Addr;
    using CPU_Common::Phy_Addr;

    class Context
    {
    public:
        Context() {}
        Context(Log_Addr usp, Log_Addr ulr, Reg flags, Log_Addr  lr, Log_Addr pc): _usp(usp), _ulr(ulr), _flags(flags), _lr(lr), _pc(pc) {
            if(Traits<Build>::hysterically_debugged || Traits<Thread>::trace_idle) {
                _r0 = 0; _r1 = 1; _r2 = 2; _r3 = 3; _r4 = 4; _r5 = 5; _r6 = 6; _r7 = 7; _r8 = 8; _r9 = 9; _r10 = 10; _r11 = 11; _r12 = 12;
            }
        }

    public:
        Reg _usp;     // usp (only used in multitasking)
        Reg _ulr;     // ulr (only used in multitasking)
        Reg _flags;
        Reg _r0;
        Reg _r1;
        Reg _r2;
        Reg _r3;
        Reg _r4;
        Reg _r5;
        Reg _r6;
        Reg _r7;
        Reg _r8;
        Reg _r9;
        Reg _r10;
        Reg _r11;
        Reg _r12;
        Reg _lr;
        Reg _pc;
    };

    // Interrupt Service Routines
    typedef void (* ISR)();

    // Fault Service Routines (exception handlers)
    typedef void (* FSR)();

protected:
    ARMv8() {};

public:
    static Log_Addr pc() { Reg r; ASM("nop"); return r; } // due to RISC pipelining, PC is read with a +8 (4 for thumb) offset

    static Log_Addr sp() { Reg r; ASM("nop"); return r; }
    static void sp(Log_Addr sp) { ASM("nop"); ASM("nop"); }

    static Reg fr() { Reg r; ASM("nop"); return r; }
    static void fr(Reg r) {  ASM("nop"); }

    static Log_Addr ra() { Reg r; ASM("nop"); return r; } // due to RISC pipelining, PC is read with a +8 (4 for thumb) offset

    static void halt() { ASM("nop"); }

    template<typename T>
    static T tsl(volatile T & lock) {
        register T old;
        /*register T one = 1;*/ //TODO
        ASM("nop");
        return old;
    }

    template<typename T>
    static T finc(volatile T & value) {
        register T old;
        if(sizeof(T) == sizeof(Reg8))
            ASM("nop");
        else if(sizeof(T) == sizeof(Reg16))
            ASM("nop");
        else
            ASM("nop");
        return old - 1;
    }

    template<typename T>
    static T fdec(volatile T & value) {
        register T old;
        if(sizeof(T) == sizeof(Reg8))
            ASM("nop");
        else if(sizeof(T) == sizeof(Reg16))
            ASM("nop");
        else
            ASM("nop");
        return old + 1;
    }

    template <typename T>
    static T cas(volatile T & value, T compare, T replacement) {
        register T old;
        if(sizeof(T) == sizeof(Reg8))
            ASM("nop");
        else if(sizeof(T) == sizeof(Reg16))
            ASM("nop");
        else
            ASM("nop");
        return old;
    }

    // ARMv8 specifics
    static Reg r0() { Reg r /*TODO*/ = 0; ASM("nop"); return r; }
    static void r0(Reg r) { ASM("nop"); }

    static Reg r1() { Reg r /*TODO*/ = 0; ASM("nop"); return r; }
    static void r1(Reg r) { ASM("nop"); }

    static Reg sctlr() { Reg r /*TODO*/ = 0; ASM("nop"); return r; }
    static void sctlr(Reg r) {  ASM("nop"); }

    static Reg actlr() { Reg r /*TODO*/ = 0; ASM("nop"); return r; }
    static void actlr(Reg r) {  ASM("nop"); }

    static void dsb() { ASM("nop"); }
    static void isb() { ASM("nop"); }

    static void svc() { ASM("nop"); }
};

class ARMv8_M: public ARMv8
{
public:
    static const bool thumb = true;

    // CPU Flags
    typedef Reg Flags;
    enum {
        FLAG_THUMB      = 1 << 24,      // Thumb state
        FLAG_Q          = 1 << 27,      // DSP Overflow
        FLAG_V          = 1 << 28,      // Overflow
        FLAG_C          = 1 << 29,      // Carry
        FLAG_Z          = 1 << 30,      // Zero
        FLAG_N          = 1 << 31       // Negative
    };

    // Exceptions
    typedef Reg Exception_Id;
    enum {                      // Priority
        EXC_RESET       = 1,    // -3 (highest)
        EXC_NMI         = 2,    // -2
        EXC_HARD        = 3,    // -1
        EXC_MPU         = 4,    // programmable
        EXC_BUS         = 5,    // programmable
        EXC_USAGE       = 6,    // programmable
        EXC_SVCALL      = 11,   // programmable
        EXC_DEBUG       = 12,   // programmable
        EXC_PENDSV      = 14,   // programmable
        EXC_SYSTICK     = 15    // programmable
    };

    // CPU Context
    class Context: public ARMv8::Context
    {
    public:
        Context() {}
        Context(Log_Addr entry, Log_Addr exit, Log_Addr usp): ARMv8::Context(usp, exit | thumb, FLAG_THUMB, exit | thumb, entry | thumb) {}
    };

protected:
    ARMv8_M() {};

public:
    static unsigned int id() { return 0; }
    static unsigned int cores() { return 1; }

    static void int_enable()  { ASM("nop"); }
    static void int_disable() { ASM("nop"); }
    static bool int_enabled() { return !int_disabled(); }
    static bool int_disabled() {
        bool disabled;
        ASM("nop");
        return disabled;
    }

    static void smp_barrier(unsigned long cores = cores()) { assert(cores == 1); }

    static Reg pd() { return 0; }       // no MMU
    static void pd(Reg r) {}            // no MMU

    static void flush_tlb() {}          // no MMU
    static void flush_tlb(Reg r) {}     // no MMU

    // ARMv8-M specifics
    static Flags flags() { Reg r; ASM("nop"); return r; }
    static void flags(Flags r) {  ASM("nop"); }

    static void psr_to_r12() { ASM("nop"); }
    static void r12_to_psr() {  ASM("nop"); }
};

class ARMv8_A: public ARMv8
{
public:
    static const bool thumb = false;

    // CPU Flags
    typedef Reg Flags;
    enum {
        FLAG_M          = 0x1f << 0,       // Processor Mode (5 bits)
        FLAG_T          = 1    << 5,       // Thumb state
        FLAG_F          = 1    << 6,       // FIQ disable
        FLAG_I          = 1    << 7,       // IRQ disable
        FLAG_A          = 1    << 8,       // Imprecise Abort disable
        FLAG_E          = 1    << 9,       // Endianess (0 ->> little, 1 -> big)
        FLAG_GE         = 0xf  << 16,      // SIMD Greater than or Equal (4 bits)
        FLAG_J          = 1    << 24,      // Jazelle state
        FLAG_Q          = 1    << 27,      // Underflow and/or DSP saturation
        FLAG_V          = 1    << 28,      // Overflow
        FLAG_C          = 1    << 29,      // Carry
        FLAG_Z          = 1    << 30,      // Zero
        FLAG_N          = 1    << 31,      // Negative

        // FLAG_M values
        MODE_USR        = 0x10,
        MODE_FIQ        = 0x11,
        MODE_IRQ        = 0x12,
        MODE_SVC        = 0x13,
        MODE_ABORT      = 0x17,
        MODE_UNDEFINED  = 0x1b,
        MODE_SYS        = 0x1f
    };

    // Exceptions
    typedef Reg Exception_Id;
    enum {
        EXC_START                   = 1,
        EXC_UNDEFINED_INSTRUCTION   = 2,
        EXC_SWI                     = 3,
        EXC_PREFETCH_ABORT          = 4,
        EXC_DATA_ABORT              = 5,
        EXC_RESERVED                = 6,
        EXC_IRQ                     = 7,
        EXC_FIQ                     = 8
    };

    enum {
        CLI_DOMAIN = 0x55555555, // 0b01 - Client, all memory domains check for memory access permission
        MNG_DOMAIN = 0xFFFFFFFF  // 0b11 - Manager, memory access permissions are not checked
    };

    // SCTLR bits
    enum {
        MMU_ENABLE  = 1 << 0,  // MMU enable
        DCACHE      = 1 << 2,  // Data cache enable
        BRANCH_PRED = 1 << 11, // Z bit, branch prediction enable
        ICACHE      = 1 << 12, // Instruction cache enable
        AFE         = 1 << 29  // Access Flag enable
    };

    // ACTLR bits
    enum {
        DCACHE_PREFE = 1 << 2, // DCache prefetch Enabled
        SMP          = 1 << 6 // SMP bit
    };

    // CPU Context
    class Context: public ARMv8::Context
    {
    public:
        Context() {}
        Context(Log_Addr entry, Log_Addr exit, Log_Addr usp): ARMv8::Context(usp, exit | thumb, multitask ? (usp ? MODE_USR : MODE_SVC) : MODE_SVC, exit | thumb, entry | thumb) {}
    };

protected:
    ARMv8_A() {};

public:
    static Flags flags() { return cpsr(); }
    static void flags(Flags flags) { cpsr(flags); }

    static unsigned int id() {
        Reg id;
        id = 13; // TODO
        ASM("nop");
        return id & 0x3;
    }

    static unsigned int cores() {
        if(Traits<Build>::MODEL == Traits<Build>::Raspberry_Pi3) {
            return Traits<Build>::CPUS;
        } else {
            Reg n;
            ASM("nop");
            return (n & 0x3) + 1;
        }
    }

    static void int_enable() {  flags(flags() & ~(FLAG_F | FLAG_I)); }
    static void int_disable() { flags(flags() | (FLAG_F | FLAG_I)); }

    static bool int_enabled() { return !int_disabled(); }
    static bool int_disabled() { return flags() & (FLAG_F | FLAG_I); }

    static void smp_barrier(unsigned long cores = cores()) { CPU_Common::smp_barrier<&finc>(cores, id()); }

    static void fpu_save() {    ASM("nop"); }
    static void fpu_restore() { ASM("nop"); }

    // ARMv8-A specifics
    static Reg cpsr() { Reg r /*TODO*/ = 0; ASM("nop"); return r; }
    static void cpsr(Reg r) { ASM("nop"); }

    static Reg cpsrc() { Reg r /*TODO*/ = 0; ASM("nop"); return r; }
    static void cpsrc(Reg r) { ASM("nop"); }

    static void psr_to_r12() { ASM("nop"); }
    static void r12_to_psr() { ASM("nop"); }

    static void save_regs(bool ret = false) {
        if(ret)
            ASM("nop");
        else
            ASM("nop");
    }

    static void restore_regs(bool ret = false) {
        if(ret)
            ASM("nop");  // including PC in ldmfd cause a mode change to the mode given by PSR (the mode the CPU was before the interrupt)
        else
            ASM("nop");
    }

    static void mode(unsigned int m) { ASM("nop"); }

    static void svc_enter(unsigned int from, bool ret = true) {
        mode(MODE_SVC);                 // go to SVC mode to save context
        save_regs(ret);                 // save current context (lr, sp and spsr are banked registers)
        mode(from);                     // go back to mode "from" to recover LR and PSR
        ASM("nop");          // r1 = return address when entering mode "from"
        ASM("nop");            // r2 = "from"_spsr (to be visible at SVC)
        mode(MODE_SVC);                 // go to SVC mode once again
        if(ret) {                       // if we will return, then
            ASM("nop");   // overwrite the saved PC with r1, which contains the recovered return address
            ASM("nop");           // push the recovered PSR
        }
        // upon return, r1 (LR) and r2 (PSR) are preserved and can be further used
    }

    static void svc_leave() {
        ASM("nop");            // pop saved mode "from" PSR into SVC_spsr
        ASM("nop");
        restore_regs(true);              // restore the context, eventually changing back to "from" mode and jumping to the saved return address
    }

    static void svc_stay() { restore_regs(false); }

    static Reg elr_hyp() { Reg r; ASM("nop"); return r; }
    static void elr_hyp(Reg r) { ASM("nop"); }

    static void ldmia() { ASM("nop"); }
    static void stmia() { ASM("nop"); }

    // CP15 operations
    static Reg ttbr0() { Reg r; r = 2; /*TODO remover atrib*/ ASM ("nop"); return r; }
    static void ttbr0(Reg r) {  ASM ("nop"); }

    static Reg ttbcr() { Reg r; ASM ("nop"); return r; }
    static void ttbcr(Reg r) {  ASM ("nop"); }

    static Reg dacr() { Reg r; ASM ("nop"); return r; }
    static void dacr(Reg r) {  ASM ("nop"); }

    static Reg pd() { return ttbr0(); }
    static void pd(Reg r) {  ttbr0(r); }

    static void flush_tlb() {      ASM("nop"); } // TLBIALL - invalidate entire unifed TLB
    static void flush_tlb(Reg r) { ASM("nop"); }

    static void flush_branch_predictors() { ASM("nop"); }

    static void flush_caches() {
        ASM("nop");
    }

    static void enable_fpu() {
        // This code assumes a compilation with mfloat-abi=hard and does not care for context switches
        ASM("nop");
    }

};

#ifndef __armv7_h

class CPU: public SWITCH<Traits<Build>::MODEL, CASE<Traits<Build>::eMote3, ARMv8_M, CASE<Traits<Build>::LM3S811, ARMv8_M, CASE<DEFAULT, ARMv8_A>>>>::Result
{
    friend class Init_System;

private:
    typedef SWITCH<Traits<Build>::MODEL, CASE<Traits<Build>::eMote3, ARMv8_M, CASE<Traits<Build>::LM3S811, ARMv8_M, CASE<DEFAULT, ARMv8_A>>>>::Result Base;

public:
    // CPU Native Data Types
    using ARMv8::Reg8;
    using ARMv8::Reg16;
    using ARMv8::Reg32;
    using ARMv8::Reg64;
    using ARMv8::Reg;
    using ARMv8::Log_Addr;
    using ARMv8::Phy_Addr;

    // CPU Context
    class Context: public Base::Context
    {
    public:
        Context() {}
        Context(Log_Addr entry, Log_Addr exit, Log_Addr usp): Base::Context(entry, exit, usp) {}

        void save() volatile;  //__attribute__ ((naked));
        void load() const volatile;

        friend Debug & operator<<(Debug & db, const Context & c) {
            db << hex
               << "{r0="  << c._r0
               << ",r1="  << c._r1
               << ",r2="  << c._r2
               << ",r3="  << c._r3
               << ",r4="  << c._r4
               << ",r5="  << c._r5
               << ",r6="  << c._r6
               << ",r7="  << c._r7
               << ",r8="  << c._r8
               << ",r9="  << c._r9
               << ",r10=" << c._r10
               << ",r11=" << c._r11
               << ",r12=" << c._r12
               << ",sp="  << &c
               << ",lr="  << c._lr
               << ",pc="  << c._pc
               << ",psr=" << c._flags
               << ",usp=" << c._usp
               << ",ulr=" << c._ulr
               << "}" << dec;
            return db;
        }
    };

public:
    CPU() {}

    using ARMv8::pc;
    using ARMv8::ra;
    using ARMv8::sp;
    using ARMv8::fr;

    using Base::id;
    using Base::cores;

    static Hertz clock() { return _cpu_clock; }
    static void clock(const Hertz & frequency); // defined along with each machine's IOCtrl
    static Hertz max_clock();
    static Hertz min_clock();
    static Hertz bus_clock() { return _bus_clock; }

    using Base::int_enable;
    using Base::int_disable;
    using Base::int_enabled;
    using Base::int_disabled;

    using ARMv8::halt;

    using Base::fpu_save;
    using Base::fpu_restore;

    using ARMv8::tsl;
    using ARMv8::finc;
    using ARMv8::fdec;
    using ARMv8::cas;

    static void switch_context(Context ** o, Context * n); //__attribute__ ((naked));

    template<typename ... Tn>
    static Context * init_stack(Log_Addr usp, Log_Addr sp, void (* exit)(), int (* entry)(Tn ...), Tn ... an) {
        sp -= sizeof(Context);
        Context * ctx = new(sp) Context(entry, exit, usp); // init_stack is called with usp = 0 for kernel threads
        init_stack_helper(&ctx->_r0, an ...);
        return ctx;
    }

    // In ARMv8, the main thread of each task gets parameters over registers, not the stack, and they are initialized by init_stack.
    template<typename ... Tn>
    static Log_Addr init_user_stack(Log_Addr usp, void (* exit)(), Tn ... an) { return usp; }

    static void syscall(void * message);
    static void syscalled();

    using CPU_Common::htole64;
    using CPU_Common::htole32;
    using CPU_Common::htole16;
    using CPU_Common::letoh64;
    using CPU_Common::letoh32;
    using CPU_Common::letoh16;

    using CPU_Common::htobe64;
    using CPU_Common::htobe32;
    using CPU_Common::htobe16;
    using CPU_Common::betoh64;
    using CPU_Common::betoh32;
    using CPU_Common::betoh16;

    using CPU_Common::htonl;
    using CPU_Common::htons;
    using CPU_Common::ntohl;
    using CPU_Common::ntohs;

private:
    template<typename Head, typename ... Tail>
    static void init_stack_helper(Log_Addr sp, Head head, Tail ... tail) {
        *static_cast<Head *>(sp) = head;
        init_stack_helper(sp + sizeof(Head), tail ...);
    }
    static void init_stack_helper(Log_Addr sp) {}

    static void context_load_helper();

    static void init();

private:
    static unsigned int _cpu_clock;
    static unsigned int _bus_clock;
};

inline CPU::Reg64 htole64(CPU::Reg64 v) { return CPU::htole64(v); }
inline CPU::Reg32 htole32(CPU::Reg32 v) { return CPU::htole32(v); }
inline CPU::Reg16 htole16(CPU::Reg16 v) { return CPU::htole16(v); }
inline CPU::Reg64 letoh64(CPU::Reg64 v) { return CPU::letoh64(v); }
inline CPU::Reg32 letoh32(CPU::Reg32 v) { return CPU::letoh32(v); }
inline CPU::Reg16 letoh16(CPU::Reg16 v) { return CPU::letoh16(v); }

inline CPU::Reg64 htobe64(CPU::Reg64 v) { return CPU::htobe64(v); }
inline CPU::Reg32 htobe32(CPU::Reg32 v) { return CPU::htobe32(v); }
inline CPU::Reg16 htobe16(CPU::Reg16 v) { return CPU::htobe16(v); }
inline CPU::Reg64 betoh64(CPU::Reg64 v) { return CPU::betoh64(v); }
inline CPU::Reg32 betoh32(CPU::Reg32 v) { return CPU::betoh32(v); }
inline CPU::Reg16 betoh16(CPU::Reg16 v) { return CPU::betoh16(v); }

inline CPU::Reg32 htonl(CPU::Reg32 v)   { return CPU::htonl(v); }
inline CPU::Reg16 htons(CPU::Reg16 v)   { return CPU::htons(v); }
inline CPU::Reg32 ntohl(CPU::Reg32 v)   { return CPU::ntohl(v); }
inline CPU::Reg16 ntohs(CPU::Reg16 v)   { return CPU::ntohs(v); }

#endif

__END_SYS

#endif
