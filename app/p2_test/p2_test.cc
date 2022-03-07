 #include <utility/ostream.h>
#include <memory.h>
#include <architecture.h>
#include <process.h>
#include <system.h>
 
 using namespace EPOS;
 
 OStream cout;
 
void adress_space_test();
void segment_test();

 int main()
 {
    cout << "P2 segment, adress space tests" << endl;
 
    cout << "1: segment_test" << endl;
    segment_test();

    cout << "2: address_space_test" << endl;
    adress_space_test();

    return 0;
 }

void segment_test()
{
	unsigned int seg1_size = 0x10000;
	unsigned int seg2_size = 0x100000;

	Segment * seg1 = new (SYSTEM) Segment(seg1_size);
	Segment * seg2 = new (SYSTEM) Segment(seg2_size);

	assert(seg1_size == seg1->size());
	assert(seg2_size == seg2->size());

	delete seg1;
	delete seg2;

	cout << "Test segment ok!" << endl;

}

void adress_space_test()
{
	unsigned int seg1_size = 0x10000;
	unsigned int seg2_size = 0x100000;
	Address_Space as(MMU::current());

	Segment * seg1 = new (SYSTEM) Segment(seg1_size);
	Segment * seg2 = new (SYSTEM) Segment(seg2_size);

	CPU::Log_Addr la1 = as.attach(seg1);
	CPU::Log_Addr la2 = as.attach(seg2);

	cout << "la1: " << la1 << endl;
	cout << "la2: " << la2 <<endl;

	assert(seg1_size == seg1->size());
	assert(seg2_size == seg2->size());

	as.detach(seg1);
	as.detach(seg2);

	delete seg1;
	delete seg2;

	cout << "Test address space ok!" << endl;
}