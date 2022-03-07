// EPOS Memory Declarations

#ifndef __memory_stub_h
#define __memory_stub_h

#include <stubs/stub.h>
#include <architecture.h>

#define SEGMENT_ID EPOS::S::SEGMENT_ID

class Segment_Stub: public Stub_Common
{
public:

    friend class Address_Space_Stub;
    friend class Task_Stub;

    typedef EPOS::S::CPU::Phy_Addr Phy_Addr;
    typedef EPOS::S::MMU::Flags Flags;

public:
    Segment_Stub(unsigned int bytes, Flags flags = Flags::APP) {
        _segment = reinterpret_cast<void *>(stub_syscall(SEGMENT_ID, Message::Method_Id::CONSTRUCTOR1, bytes, flags));
    }

    Segment_Stub(Phy_Addr phy_addr, unsigned int bytes, Flags flags) {
        _segment = reinterpret_cast<void *>(stub_syscall(SEGMENT_ID, Message::Method_Id::CONSTRUCTOR2, phy_addr, bytes, flags));
    }

    Segment_Stub(Phy_Addr phy_addr, unsigned int from, unsigned int to, Flags flags) {
        _segment = reinterpret_cast<void *>(stub_syscall(SEGMENT_ID, Message::Method_Id::CONSTRUCTOR3, phy_addr, from, to, flags));
    }

    ~Segment_Stub() {
        stub_syscall(SEGMENT_ID, Message::Method_Id::DESTROY, _segment);
    }

    long unsigned int size() const {
        return reinterpret_cast<long unsigned int>(stub_syscall(SEGMENT_ID, Message::Method_Id::SEGMENT_SIZE, _segment));
    }

    Phy_Addr phy_address() const {
        return static_cast<Phy_Addr>(stub_syscall(SEGMENT_ID, Message::Method_Id::SEGMENT_PHY_ADDRESS, _segment));
    }

    int resize(int amount) {
        return static_cast<int>(stub_syscall(SEGMENT_ID, Message::Method_Id::SEGMENT_RESIZE, _segment, amount));
    }

private:
    Segment_Stub(void * segment) : _segment(segment) { }

private:
    void * _segment;

};

#define ADDRESS_SPACE_ID EPOS::S::ADDRESS_SPACE_ID

class Address_Space_Stub: public Stub_Common
{
public:
    friend class Task_Stub;

    typedef EPOS::S::CPU::Phy_Addr Phy_Addr;
    typedef EPOS::S::CPU::Log_Addr Log_Addr;
    typedef EPOS::S::MMU MMU;
public:
    Address_Space_Stub() {
        _address_space = reinterpret_cast<void *>(stub_syscall(ADDRESS_SPACE_ID, Message::CONSTRUCTOR1));
    }

    Address_Space_Stub(MMU::Page_Directory * pd) {
        _address_space = reinterpret_cast<void *>(stub_syscall(ADDRESS_SPACE_ID, Message::CONSTRUCTOR2, pd));
    }

    ~Address_Space_Stub() {
        stub_syscall(ADDRESS_SPACE_ID, Message::DESTROY, _address_space);
    }

    Log_Addr attach(Segment_Stub * seg) {
        void * _seg = seg->_segment;
        return static_cast<Log_Addr>(stub_syscall(ADDRESS_SPACE_ID, Message::ADDRESS_SPACE_ATTACH1, _address_space, _seg));
    }

    Log_Addr attach(Segment_Stub * seg, unsigned int from) {
        void * _seg = seg->_segment;
        return static_cast<Log_Addr>(stub_syscall(ADDRESS_SPACE_ID, Message::ADDRESS_SPACE_ATTACH2, _address_space, _seg, from));
    }

    Log_Addr attach(Segment_Stub * seg, Log_Addr addr) {
        void * _seg = seg->_segment;
        return static_cast<Log_Addr>(stub_syscall(ADDRESS_SPACE_ID, Message::ADDRESS_SPACE_ATTACH3, _address_space, _seg, addr));
    }

    void detach(Segment_Stub * seg) {
        void * _seg = seg->_segment;
        stub_syscall(ADDRESS_SPACE_ID, Message::ADDRESS_SPACE_DETACH1, _address_space, _seg);
    }

    void detach(Segment_Stub * seg, Log_Addr addr) {
        void * _seg = seg->_segment;
        stub_syscall(ADDRESS_SPACE_ID, Message::ADDRESS_SPACE_DETACH2, _address_space, _seg, addr);
    }

    Phy_Addr physical(Log_Addr address) {
        return static_cast<Log_Addr>(stub_syscall(ADDRESS_SPACE_ID, Message::ADDRESS_SPACE_PHYSICAL, _address_space, address));
    }

    private:
    Address_Space_Stub(void * address_space) : _address_space(address_space) { }

    private:
    void * _address_space;
};

#define Segment Segment_Stub
#define Address_Space Address_Space_Stub

#endif
