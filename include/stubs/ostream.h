#ifndef __ostream_stub_h
#define __ostream_stub_h

#include <stubs/stub.h>

#define UTILITY_ID EPOS::S::UTILITY_ID

class OStream_Stub : public Stub_Common
{
protected:
    OStream_Stub() { }

public:
    static void print(const char * s) {
        stub_syscall(UTILITY_ID, Message::OS_STREAM_PRINT, s);
    }
};

#define OStream OStream_Stub

#endif