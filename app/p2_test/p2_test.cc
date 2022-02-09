 #include <utility/ostream.h>
#include <memory.h>
#include <architecture.h>
#include <process.h>
#include <system.h>
 
 using namespace EPOS;
 
 OStream cout;
 
void test_address_space();

void test_segment();

void test_task();

 int main()
 {
     cout << "Hello world!" << endl;
 
    // Uncomment these three function calls to test these functionalities
    // TEST 1
    test_segment();

    // TEST 2
    test_address_space();

    // TEST 3
    test_task();


    // TEST MULTITASK SCHEDULING, THIS BLOCK IS REQUIRED FOR TESTS 4 AND 5.
    /**
    unsigned int tid = Task::self()->tid();
    cout << "tid: " << tid << endl;
    if (!tid)
    	Task::fork();
	**/

    // TEST 4: Rescheduling by dispatching demonstration.
    /**
    for(int i = 0; i < 10; i++)
    {
    	if (i == 5)
    		Thread::yield();
        cout << tid << ": " << i << endl;
    }
    **/

    // TEST 5: Rescheduling by timer interruption demonstration (NOT WORKING).
    /**
    for(int i = 0;; i++)
    {
        cout << tid << ": " << i << endl;
    }
    **/
     return 0;
 }

void test_address_space()
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

	assert(0x80100000 == la1);
	assert(0x80200000 == la2);

	assert(seg1_size == seg1->size());
	assert(seg2_size == seg2->size());

	as.detach(seg1);
	as.detach(seg2);

	delete seg1;
	delete seg2;

	cout << "Test address space ok!" << endl;
}

void test_segment()
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

void test_task()
{
	System_Info * si = System::info();
	Task * t = Task::self();
	Address_Space * as = t->address_space();

	assert(as->pd() == MMU::current());

	assert(si->lm.app_code == (t->code()));
	assert(si->lm.app_data == (t->data()));

	cout << "Test task ok!" << endl;
}