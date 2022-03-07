// EPOS Component Framework - Proxy x Agent Message

#ifndef __message_h
#define __message_h

#include "architecture/cpu.h"
// extern "C" { void _syscall(void *); }

// __BEGIN_SYS

class Message
{
private:
    static const unsigned int MAX_PARAMETERS_SIZE = 256;

public:
    enum Method_Id {
        CONSTRUCTOR1,
        CONSTRUCTOR2,
        CONSTRUCTOR3,
        DESTROY,
        SELF,

        THREAD_STATE,
        THREAD_STATISTICS,
        THREAD_PRIORITY_GET,
        THREAD_PRIORITY_SET,
        THREAD_TASK,
        THREAD_JOIN,
        THREAD_PASS,
        THREAD_SUSPEND,
        THREAD_RESUME,
        THREAD_YIELD,
        THREAD_EXIT,

        SEGMENT_SIZE,
        SEGMENT_PHY_ADDRESS,
        SEGMENT_RESIZE,

        TASK_ADDRESS_SPACE,
        TASK_CODE_SEGMENT,
        TASK_DATA_SEGMENT,
        TASK_CODE,
        TASK_DATA,
        TASK_MAIN,

        SYNCHRONIZER_LOCK,
        SYNCHRONIZER_UNLOCK,

        ALARM_GET_PERIOD,
        ALARM_SET_PERIOD,
        ALARM_DELAY,

        RESET,
        FREQUENCY,

        CHRONOMETER_START,
        CHRONOMETER_LAP,
        CHRONOMETER_STOP,
        CHRONOMETER_READ,

        ADDRESS_SPACE_ATTACH1,
        ADDRESS_SPACE_ATTACH2,
        ADDRESS_SPACE_ATTACH3,
        ADDRESS_SPACE_DETACH1,
        ADDRESS_SPACE_DETACH2,
        ADDRESS_SPACE_PHYSICAL,

        OS_STREAM_PRINT,

        UNDEFINED
    };

    typedef unsigned long Method;
    typedef Method Result;
    typedef unsigned int Id;
    typedef Id Status;

public:
    Message() {}
    Message(const Message & msg) { *this = msg; }
    template<typename ... Tn>
    Message(const Id & id, const Method & m, Tn && ... an): _id(id), _method(m) { }

    const Id & id() const { return _id; }
    void id(const Id & id) { _id = id; }
    const Method & method() const { return _method; }
    void method(const Method & m) { _method = m; }
    const Result & result() const { return _method; }
    void result(const Result & r) { _method = r; }

    static void serialize(char * buffer, int index);
    template<typename T>
    static void serialize(char * buffer, int index, T* a);
    template<typename T, typename ... Tn>
    static void serialize(char * buffer, int index, T* a, Tn* ... an);

    static void deserialize(char * buffer, int index);
    template<typename T>
    static void deserialize(char * buffer, int index, T a);
    template<typename T, typename ... Tn>
    static void deserialize(char * buffer, int index, T a, Tn ... an);

    template<typename ... Tn>
    void in(Tn ... an) {
        Message::deserialize(&_params[0], 0, an ...);
    }
    template<typename ... Tn>
    void out(const Tn ... an) {
        Message::serialize(&_params[0], 0, an ...);
    }

    void act() { EPOS::S::CPU::syscall(this); }
public:
    Id _id;
    Method _method;
    char _params[MAX_PARAMETERS_SIZE];
};

template<typename T>
void Message::deserialize(char * buffer, int index, T a) {
    *(reinterpret_cast<T*>(&buffer[index])) = a;
}

template<typename T, typename ... Tn>
void Message::deserialize(char * buffer, int index, T a, Tn ... an) {
    *(reinterpret_cast<T*>(&buffer[index])) = a;
    Message::deserialize(buffer, index + sizeof(T), an...);
}

template<typename T>
void Message::serialize(char * buffer, int index, T* a) {
    *a = *(reinterpret_cast<T*>(&buffer[index]));
}

template<typename T, typename ... Tn>
void Message::serialize(char * buffer, int index, T* a, Tn* ... an) {
    *a = *(reinterpret_cast<T*>(&buffer[index]));
    Message::serialize(buffer, index + sizeof(T), an...);
}

// __END_SYS

#endif