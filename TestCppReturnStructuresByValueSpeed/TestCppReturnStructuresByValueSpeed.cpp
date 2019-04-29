// TestCppReturnStructuresByValueSpeed.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <map>
#include <string>
#include <fmt/format.h>
#include <chrono>

using namespace std;


struct ColumnInfo
{
	wstring	cmsKey;
	int				cmsType;
	int				columnLength;
	wstring	columnName;
	bool			m_bEncrypt;
	wstring	foreignTableName;
	wstring	foreignColumnName;
	wstring    foreignCreator;
	wstring	creator;
	bool	required;
	int		columnType;
	bool	fulltext;
	bool	unicode;
};

struct TableInfo
{
	TableInfo() = default;

	TableInfo(const std::wstring& name)
	{
		this->name = name;
	}

	std::wstring name;
	std::map<std::wstring, ColumnInfo> columns;
};


int main()
{
	constexpr auto iters = 100'000;

	auto t = TableInfo{L"PROFILE"};
	for (int i = 0; i < 8; i++)
	{
		auto ci = ColumnInfo
		{
			fmt::format(L"CMSKEY_VALUE_{}", i), // cmsKey
			i, // cmsType
			i * 10, //columnLength;
			fmt::format(L"COLUMN_{}",i),   //	columnName;
			true, // m_bEncrypt;
			i % 4 == 0 ? L"ANOTHERTABLE" : L"", //foreignTableName;
			i % 4 == 0 ? L"ANOTHERCOL" : L"", // foreignColumnName;
			i % 4 == 0 ? L"DOCSADM" : L"", //wstring    foreignCreator;
			L"DOCSADM", //creator
			true, //required
			2, //columnType
			false, //fulltext
			true //unicode
		};
		t.columns[fmt::format(L"Column{}", i)] = ci;
	}

	auto testStart = std::chrono::high_resolution_clock::now();

	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < iters; ++i)
	{
		auto tCopy{ t };

	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> diff = end - start;
	std::cout << fmt::format("Time to copy {0:n} table structures is {1} ms\n", iters, diff.count());

	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < iters; ++i)
	{
		auto cCopy{ t.columns[L"Column4"]};
	}
	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	std::cout << fmt::format("Time to copy {0:n} ColumnInfo structures is {1} ms\n", iters, diff.count());


	auto pti = std::make_shared<TableInfo>(t);
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < iters; ++i)
	{
		auto ptCopy{ pti };
	}
	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	std::cout << fmt::format("Time to acquire {0:n} table structure shared pointers is {1} ms\n", iters, diff.count());

	std::cout << fmt::format("Total time is {1} seconds\n", iters, std::chrono::duration<double>{std::chrono::high_resolution_clock::now() - testStart}.count());
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
