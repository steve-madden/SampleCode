// SharedPtrMultithreadedLifetimeSample.cpp 


#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <chrono>
#include <sstream>
#include <string>


using namespace std::chrono_literals;

namespace
{
	std::string GetThreadId()
	{
		std::stringstream ss;
		ss << std::hex << std::this_thread::get_id();
		return ss.str();
	}

	std::mutex s_doneMtx;
	std::condition_variable s_doneCv;
	bool s_done = false;
}

class A : public std::enable_shared_from_this<A>
{
public:
	A() = default;

	~A()
	{
		auto i = _i.load();
		printf("[%s] A::~A called. _i: %d\n", GetThreadId().c_str(), i);

		std::unique_lock lk(s_doneMtx);
		s_done = true;
		lk.unlock();
		s_doneCv.notify_all();
	}


	void StartThreads()
	{
		// Run a member function on this instance
		std::thread([this, pThis = shared_from_this()]
					{
						MemberTask(pThis);
					})
			.detach();

		// Run a static member function passing in this as a shared_ptr
		std::thread([pThis = shared_from_this()]
					{ 
						A::StaticTask(pThis); 
					})
			.detach();
	}


private:
	void MemberTask(std::shared_ptr<A> pa)
	{
//		printf("[%s] A::MemberTask Started\n", GetThreadId().c_str());
		std::this_thread::sleep_for(2s);
		++_i;
//		printf("[%s] A::MemberTask Done\n", GetThreadId().c_str());
	}

	static void StaticTask(std::shared_ptr<A> pa)
	{
//		printf("[%s] C::StaticTask Started\n", GetThreadId().c_str());
		std::this_thread::sleep_for(1s);
		++pa->_i;
//		printf("[%s] C::StaticTask Done\n", GetThreadId().c_str());
	}

	std::atomic_int _i = 0;
};

int main()
{
	using namespace std::chrono_literals;
	using namespace std::chrono;

	auto start_time = steady_clock::now();

	constexpr auto iterations = 10000;

	auto pA = std::make_shared<A>();
	for (auto i = 0; i < iterations; i++)
	{
		pA->StartThreads();
	}

	auto elapsed_ms = duration_cast<milliseconds>(steady_clock::now() - start_time);
	printf_s("%ld std::thread based threads started in in %lld ms (%lld/sec)\n", 
			 iterations * 2, elapsed_ms.count(), (iterations * 2 * 1000LL) / elapsed_ms.count());

	pA.reset();
	printf("[%s] Main thread has released A and is waiting for done signal\n", GetThreadId().c_str());
	
	// wait for pA destructor to run
	{
		std::unique_lock lk(s_doneMtx);
		s_doneCv.wait(lk, [] {return s_done; });
	}
	printf("[%s] Main thread has received the done signal\n", GetThreadId().c_str());

	elapsed_ms = duration_cast<std::chrono::milliseconds>(steady_clock::now() - start_time);
	printf_s("%ld std::thread based threads completed in in %lld ms (%lld/sec)\n", 
			 iterations * 2, elapsed_ms.count(), (iterations * 2 * 1000LL) / elapsed_ms.count());

	return 0;
}
