// DocumentPipelineTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <agents.h>
#include <string>
#include <iostream>

#include "DecompressionAgent.h"
#include "DecryptionAgent.h"

using namespace concurrency;





int main()
{
	unbounded_buffer<std::string> documentBuffer{};

	decompression_agent stage_1{ documentBuffer };
	stage_1.start();

	decryption_agent stage_2{ stage_1.Output() };
	stage_2.start();

	asend(documentBuffer, std::string("Start of buffer"));
	
	std::string in{};
	while (true)
	{
		std::getline(std::cin, in);
		asend(documentBuffer, in);

		if (in.size() == 0)
			break;
	}

	agent::wait(&stage_2);

	std::string out{};
	while (try_receive(stage_2.Output(), out))
		printf("%s\n", out.c_str());

    std::cout << "Done writing!\n"; 
}
