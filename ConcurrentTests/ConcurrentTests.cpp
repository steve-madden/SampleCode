// ConcurrentTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <ppltasks.h>
#include <concrt.h>
#include <agents.h>


using namespace concurrency;
using namespace std;
namespace {
	concurrency::event s_doneEvent {};
}

class C : public enable_shared_from_this<C>
{
public:
	C(){}

	~C() 
	{
		auto i = _i.load();
		printf("[%x] C::~C called. _i: %d\n", ::GetCurrentThreadId(), i);
		s_doneEvent.set();
	}

	void StartTasksBasedThreads()
	{
		auto mt = task<void>([this, pThis = shared_from_this()] {MemberTask(pThis); });
		auto st = task<void>([pThis = shared_from_this()] {C::StaticTask(pThis); });
	}


private:
	void MemberTask(shared_ptr<C> pc)
	{
		// this will only return after 2 seconds as the destructor sets the event
		Sleep(2000);
		++_i;
	}

	static void StaticTask(shared_ptr<C> pc)
	{
		// this will only return after 1 second as the destructor sets the event
		Sleep(1000);
		++pc->_i;
	}

	atomic_int _i = 0;
};

int main()
{
	using clock = std::chrono::steady_clock;
	auto start_time = clock::now();

	constexpr auto iterations = 1000;

	auto c = make_shared<C>();
	for (auto i = 0; i < iterations; ++i)
	{
		c->StartTasksBasedThreads();
	}

	auto elapsed = clock::now() - start_time;

	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

	printf_s("%ld Task based threads started in in %lld ms.\n", iterations*2, elapsed_ms.count());


	c.reset();
	printf("[%x] Main thread has released C and is waiting for done signal\n", ::GetCurrentThreadId());

	// wait for pA destructor to run
	s_doneEvent.wait();

	printf("[%x] Main thread has received the done signal\n", GetCurrentThreadId());

	elapsed = clock::now() - start_time;
	elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

	printf_s("%ld Task based threads completed in in %lld ms (%lld/sec)\n", iterations * 2, elapsed_ms.count(), (iterations * 2 * 1000LL) / elapsed_ms.count());


	return 0;
}

