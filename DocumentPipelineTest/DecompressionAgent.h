#pragma once

#include <agents.h>
#include <string>
#include <stdio.h>

class decompression_agent : public concurrency::agent
{
public:
	explicit decompression_agent(concurrency::ISource<std::string>& sourceBuffer ) 
		:_source(sourceBuffer)
	{
	}

	concurrency::ISource<std::string>& Output() { return _output; }


protected:
	void run()
	{
		std::string in{};
		while (true) {
			in = concurrency::receive(_source);

			if (in.size() > 0)
			{
				printf("decompression_agent: processing %s\n", in.c_str());
				in.append(".decompressed");
			}
			else
			{
				printf("decompression_agent: end of stream\n");

			}

			asend(_output, in);

			if (in.size() == 0)
				break;
		};
		done();
	}



private:
	concurrency::ISource<std::string>& _source;
	concurrency::unbounded_buffer<std::string> _output{};
};