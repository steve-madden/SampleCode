// AgentsTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <agents.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctime>


using namespace std;
using namespace concurrency;

template<class T>
class DataflowAgent : public agent
{
public:
    explicit DataflowAgent() : _source(nullptr)
    {}

    ISource<T>& Output() { return _outputBuffer; }
    void Source(ISource<T>* source) { _source = source; }

protected:
    virtual void run() {}
    ISource<T>* _source;
    unbounded_buffer<T> _outputBuffer;

private:
};


/// <summary>
/// A <c>DataflowPipeline</c> is a collection of agents of type <c>DataflowAgent</c> that
/// process messages of type <c>T</c> in a multithreaded serial pipeline.
/// </summary>
template< class T>
class DataflowPipeline
{
public:
    DataflowPipeline(){}

    void push_back(unique_ptr<DataflowAgent<T>>& theAgent)
    {
        if (_agents.empty())
        {
            theAgent->Source(&_inputBuffer);
        }
        else
        {
            theAgent->Source(&last()->Output());
        }
        _agents.push_back(std::move(theAgent));
    }

    void start_all()
    {
        for each(auto& a in _agents)
        {
            a->start();
        }
    }

    DataflowAgent<T>* last() { return _agents.empty() ? nullptr : _agents.back().get(); 
    }

    agent_status wait() 
    {
        if (!_agents.empty() && last()->status() != agent_created ) 
            return Concurrency::agent::wait(last());
        return agent_created;
    }

    bool send(const T& _Data) { return Concurrency::send<T>(_inputBuffer, _Data); }

    bool enqueue(T const& _Data) { return send(_Data); }

    ITarget<T>& InputBuffer() { return _inputBuffer; }

private:
    unbounded_buffer<T> _inputBuffer;
    vector<unique_ptr<DataflowAgent<T>>> _agents;
};



class EncryptionAgent : public DataflowAgent<string>
{
public:
    explicit EncryptionAgent(string label) 
        : DataflowAgent<string>()
        , _label(label)
    {
    }

protected:
    void run()
    {
        string s;
        while(!(s = receive(_source)).empty())
        {
            //printf("[%d] %s Processing %s\n", GetCurrentThreadId(), _label.c_str(), s.c_str());
            //concurrency::wait(1);
            asend(_outputBuffer, s.append(" " + _label + " "));
        }
        asend(_outputBuffer, s);
        done();
    }

private:
    string _label;
};




int main()
{
    event done;
    atomic<int> count(0);

    call<string> printMessage([&](string s) 
    {
        if (s.empty())
        {
            printf("Finished processing %d items\n", count.load());
            done.set();
        }
        else
        {
            ++count;
            if (count % 100'000 == 0)
            {
                printf("Processed %d items\n", count.load());
            }

        }
    });

    DataflowPipeline<string> pipeline;

    {
        unique_ptr<DataflowAgent<string>> a = make_unique<EncryptionAgent>("Pass 1"s);
        pipeline.push_back(a);
        a = make_unique<EncryptionAgent>("Pass 2"s);
        pipeline.push_back(a);
        a = make_unique<EncryptionAgent>("Pass 3"s);
        pipeline.push_back(a);
        pipeline.last()->Output().link_target(&printMessage);
    }

    pipeline.start_all();
    
    printf("[%d] Sending messages\n", GetCurrentThreadId());

    for (auto i = 0; i < 10'000'000; i++)
    {
        auto s = stringstream { };
        s << "Message " << i;
        pipeline.enqueue(s.str());
    }
    pipeline.enqueue(""s);

    printf("[%d] Finished sending messages\n", GetCurrentThreadId());

    done.wait();
    //wait(1000);

    return 0;
}

