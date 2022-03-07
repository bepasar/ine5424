#ifndef __time_stub_h
#define __time_stub_h

#include <stubs/stub.h>

#define ALARM_ID EPOS::S::ALARM_ID

class Alarm_Stub : public Stub_Common
{
public:
    typedef EPOS::S::Microsecond Microsecond;
    typedef EPOS::S::Hertz Hertz;
    typedef EPOS::S::U::Handler Handler;

public:
    Alarm_Stub(const Microsecond & time, Handler * handler, unsigned int times = 1) {
        _alarm = reinterpret_cast<void *>(stub_syscall(ALARM_ID, Message::Method_Id::CONSTRUCTOR1));
    }

    ~Alarm_Stub() {
        stub_syscall(ALARM_ID, Message::Method_Id::DESTROY, _alarm);
    }

    const Microsecond period() const {
        return static_cast<const Microsecond>(stub_syscall(ALARM_ID, Message::Method_Id::ALARM_GET_PERIOD, _alarm));
    }

    void period(const Microsecond & p) {
        stub_syscall(ALARM_ID, Message::Method_Id::ALARM_SET_PERIOD, _alarm, p);
    }

    void reset() {
        stub_syscall(ALARM_ID, Message::Method_Id::RESET, _alarm);
    }

    static Hertz frequency() {
        return static_cast<const Hertz>(stub_syscall(ALARM_ID, Message::Method_Id::FREQUENCY));
    }

    static void delay(const Microsecond & time) {
        stub_syscall(ALARM_ID, Message::Method_Id::ALARM_DELAY, time);
    }

private:
    void * _alarm;
};

class Delay_Stub
{
public:
    typedef EPOS::S::Microsecond Microsecond;

public:
    Delay_Stub(const Microsecond & time) { Alarm_Stub::delay(time); }
};

#define CHRONOMETER_ID EPOS::S::CHRONOMETER_ID

class Chronometer_Stub: public Stub_Common
{
public:
    typedef EPOS::S::Microsecond Microsecond;
    typedef EPOS::S::Hertz Hertz;

public:
    Chronometer_Stub() {
        _chronometer = reinterpret_cast<void *>(stub_syscall(CHRONOMETER_ID, Message::CONSTRUCTOR1));
    }

    ~Chronometer_Stub() {
        stub_syscall(CHRONOMETER_ID, Message::DESTROY, _chronometer);
    }

    Hertz frequency() {
        return static_cast<Hertz>(stub_syscall(CHRONOMETER_ID, Message::FREQUENCY, _chronometer));
    }

    void reset() {
        stub_syscall(CHRONOMETER_ID, Message::RESET, _chronometer);
    }

    void start() {
        stub_syscall(CHRONOMETER_ID, Message::CHRONOMETER_START, _chronometer);
    }

    void lap() {
        stub_syscall(CHRONOMETER_ID, Message::CHRONOMETER_LAP, _chronometer);
    }

    void stop() {
        stub_syscall(CHRONOMETER_ID, Message::CHRONOMETER_STOP, _chronometer);
    }

    Microsecond read() {
        return static_cast<Microsecond>(stub_syscall(CHRONOMETER_ID, Message::CHRONOMETER_READ, _chronometer));
    }

private:
    void * _chronometer;
};

#define Alarm Alarm_Stub
#define Delay Delay_Stub
#define Chronometer Chronometer_Stub

#endif
