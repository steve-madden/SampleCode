// SafeArraySamples.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <atlsafe.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <chrono>
#include <ppl.h>

using namespace std;
using namespace concurrency;


auto to_variant_safearray(const std::string& str) -> CComVariant
{
	CComSafeArray<char> sa(static_cast<ULONG>(str.length()+1));
	strcpy_s(reinterpret_cast<char*>(sa.m_psa->pvData), sa.GetCount(), str.c_str());

	VARIANT v;
	VariantInit(&v);
	v.vt = VT_ARRAY | VT_I1;
	v.parray = sa.Detach();
	return v;
}


auto GetCriteriaSafeArray(LPSAFEARRAY* ppCriteriaSafeArray) -> HRESULT
{
	if (ppCriteriaSafeArray == nullptr || *ppCriteriaSafeArray != nullptr)
		return E_INVALIDARG;

	static auto criteria = unordered_map<wstring, CComVariant>
	{
		{ L"id", 0L },
	    { L"name", L"Alice Smith" }, 
	    { L"email", L"alice.smith@contoso.com"},
	    { L"isAdmin", true },
		{ L"shortUtf8value", to_variant_safearray("shortString") },
		{ L"longUtf8value", to_variant_safearray("a string that is longer than the short string optimization") },
	};

	static atomic<unsigned long> id{ 0 };

	auto hr = HRESULT{ S_OK };

	criteria[L"id"] = ++id;

	// Define the array bound structure
	CComSafeArrayBound bound[2];
	bound[0].SetCount((ULONG)criteria.size());
	bound[0].SetLowerBound(0);
	bound[1].SetCount(2);
	bound[1].SetLowerBound(0);
	auto sa = std::make_unique<CComSafeArray<VARIANT>>(bound, 2);

	long alIndex[2] = { 0 };
	long mapPropertyIndex{ 0 };
	for (auto& item : criteria)
	{
		alIndex[0] = mapPropertyIndex++;
		alIndex[1] = 0;
		auto ccv = CComVariant(item.first.c_str());
		hr = sa->MultiDimSetAt(alIndex, ccv);
		if (FAILED(hr))
			break;
		alIndex[1] = 1;
		hr = sa->MultiDimSetAt(alIndex, item.second);
		if (FAILED(hr))
			break;
	}
	if (SUCCEEDED(hr))
	{
		*ppCriteriaSafeArray = sa->Detach();
	}
	// if it didn't succeed, we don't detatch and the CComSafeArray cleans up.

	return hr;
}

auto main() -> int
{
	using clock = std::chrono::steady_clock;
	auto start_time = clock::now();

	constexpr auto numberOfPasses = 1'000'000L;

	parallel_for(0L, numberOfPasses, [](int i)
	{
		if (i % 100'000 == 0)
			printf_s("%d\n", i);

		auto criteriaSafeArray = CComSafeArray<VARIANT>{};
		LPSAFEARRAY pRawCriteriaSafeArray{ nullptr };
 
		if (SUCCEEDED(GetCriteriaSafeArray(&pRawCriteriaSafeArray)))
		{
			// First, lets attach the raw safe array to a CComSafeArray
			// this will ensure it gets correctly destroyed if an exception is thrown
			criteriaSafeArray.Attach(pRawCriteriaSafeArray);
			pRawCriteriaSafeArray = nullptr;

			// Create a kvp map for the safe array values
			auto returnedValues = std::unordered_map<wstring, CComVariant>{};

#if 0
			// now lets use the CComSafeArray methods to move it into a kvp map
			// this runs at about 400K/sec on an i5 6300U @ 2.4GHz
			long alIndex[2]{ 0 };
			for (; alIndex[0] < criteriaSafeArray.GetCount(0); alIndex[0]++)
			{
				auto varKey = CComVariant{};
				alIndex[1] = 0;
				auto hr = criteriaSafeArray.MultiDimGetAt(alIndex, varKey);

				auto varValue = CComVariant{};
				alIndex[1] = 1;
				hr = criteriaSafeArray.MultiDimGetAt(alIndex, varValue);
				returnedValues[varKey.bstrVal] = varValue;
			}

#else
			// Alternate direct access Way - minimal copies of variants and their bstrs
			// This runs at about 950K/sec including getting the values using two cores
			VARIANT* pData;
			::SafeArrayAccessData(criteriaSafeArray, (void**)&pData);
			auto propertyCount = criteriaSafeArray.GetCount(0);
			for (ULONG propertyIndex = 0; propertyIndex < propertyCount; propertyIndex++)
			{
				// the array is layed out K1 K2 K3 V1 V2 V3
				VARIANT* k = &pData[propertyIndex];
				VARIANT* v = &pData[propertyCount + propertyIndex];
				//returnedValues.emplace(k->bstrVal, *v);  // 383K/sec
				returnedValues[k->bstrVal] = *v;  // 550K/sec
			}
			::SafeArrayUnaccessData(criteriaSafeArray);
#endif
		}
	});

	auto elapsed = clock::now() - start_time;

	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

	printf_s("%ld safearrays in %lld ms (%lld/sec)\n", numberOfPasses, elapsed_ms.count(), (numberOfPasses * 1000LL) / elapsed_ms.count() );
	//char exitchars[10] = { 0 };
	//auto ps = gets_s(exitchars);
    return 0;
}

