// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <array>
#include <memory>


struct S
{
	char b{ '{' };
	uint32_t i{ 0xEEEE };
	int64_t x{ -1ll };
	char ca[4]{'a','b','c','d'};
	std::array<uint8_t, 8> sa{};
};

void TestS()
{
	S s{};
	auto ssize = sizeof(s);
	auto asize = sizeof(s.ca);
	auto sasize = sizeof(s.sa);
	s.sa.fill('a');

	auto p = std::make_shared<S>();
	p->sa.fill('a');

	std::cout << "Hello World!\n";
}



int main()
{
}
