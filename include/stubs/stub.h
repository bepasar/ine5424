
#ifndef __stub_common_
#define __stub_common_

#include <interface/message.h>

class Segment_Stub;
class Address_Space_Stub;
class Thread_Stub;
class Task_Stub;
class Display_Stub;
class Semaphore_Stub;
class Mutex_Stub;
class Alarm_Stub;
class Delay_Stub;
class Chronometer_Stub;

class Stub_Common {
public:
    template<typename ... Tn>
    static inline Message::Result stub_syscall(const unsigned int & id, Message::Method_Id method, Tn ... an) {
        Message message(id, method);
        message.in(an...);
        message.act();
        
        if (message.result() == Message::UNDEFINED) {
            EPOS::S::U::db<Stub_Common>(EPOS::S::U::WRN) << "Syscall error, id=" << id << ", method=" << method << EPOS::S::U::endl;
        }
        return message.result();
    }

    static inline Message::Result stub_syscall(const unsigned int & id, Message::Method_Id method) {
        Message message(id, method);
        message.act();
        if (message.result() == Message::UNDEFINED) {
            EPOS::S::U::db<Stub_Common>(EPOS::S::U::WRN) << "Syscall error, id=" << id << ", method=" << method << EPOS::S::U::endl;
        }
        return message.result();
    }
};

#endif
