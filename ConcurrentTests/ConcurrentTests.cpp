// ConcurrentTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <ppltasks.h>

using namespace concurrency;
using namespace std;


class C : public enable_shared_from_this<C>
{
public:
	C()
	{
		m_pi = make_unique<atomic_int>(0);
	}

	~C() 
	{
		auto i = m_pi->load();
		ATLTRACE("~C destructor called - m_i=%d\n", i);
	}

	void Run()
	{
		auto mt = task<void>([this, pThis = shared_from_this()] {MemberTask(pThis); });
		auto st = task<void>([pThis = shared_from_this()] {C::StaticTask(pThis); });
	}


private:
	void MemberTask(shared_ptr<C> pc)
	{
		ATLTRACE("C::MemberTask Started\n");
		Sleep(2000);
		(*m_pi)++;
		ATLTRACE("C::MemberTask Done\n");
	}

	static void StaticTask(shared_ptr<C> pc)
	{
		ATLTRACE("C::StaticTask Started\n");
		Sleep(1000);
		++(*pc->m_pi);
		ATLTRACE("C::StaticTask Done\n");
	}

	unique_ptr<atomic_int> m_pi;
};

int main()
{
	auto c = make_shared<C>();
	c->Run();
	c.reset();
	ATLTRACE("c.reset called on main thread\n");
	Sleep(3000);
    return 0;
}

