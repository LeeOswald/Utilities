#pragma once


#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <windows.h>

//#define PERFTIMER_ENABLED


class PerformanceTimer
{
public:
	~PerformanceTimer()
	{
		LARGE_INTEGER Finished;
		::QueryPerformanceCounter(&Finished);
		auto Delta = Finished.QuadPart - m_Started.QuadPart;

		auto It = Items().find(m_Id);
		assert(It != Items().end());
		auto& w = It->second;

		PopPrev();
		
		w.Duration += Delta;
		++w.Count;

		--Ref();

		if (Ref() == 0)
		{
			LARGE_INTEGER Freq;
			::QueryPerformanceFrequency(&Freq);

			auto& Ite = Items();
			std::vector<What> Sorted;
			Sorted.reserve(Items().size());
			for (auto const &w : Ite)
			{
				Sorted.push_back(w.second);
			}
			Ite.clear();

			std::sort(Sorted.begin(), Sorted.end(), [](What const &a, What const &b) { return a.FullId < b.FullId; });

			OutputDebugStringW(L"---------------------------------------------------------------------------------------------->\n");
			for (auto const &w : Sorted)
			{
				std::wstring s;
				s.append(L"[");
				s.append(w.FullId);
				s.append(L"] ");
				
				auto Diff = w.Duration / double(Freq.QuadPart);

				wchar_t Tmp[64];
				::swprintf_s(Tmp, _countof(Tmp), L"%llu  %.3f", w.Count, Diff);
				s.append(Tmp);
				s.append(L" sec\n");

				::OutputDebugStringW(s.c_str());
			}
			OutputDebugStringW(L"<----------------------------------------------------------------------------------------------\n");
		}
	}

	PerformanceTimer(std::wstring const &Id)
		: m_Id(MakeId(Id))
	{
		++Ref();

		SetPrev(Id);

		auto It = Items().find(m_Id);
		if (It == Items().end())
		{
			Items().insert(std::make_pair(m_Id, What(Id, m_Id)));
		}

		::QueryPerformanceCounter(&m_Started);
	}


private:
	struct What
	{
		std::wstring Id;
		std::wstring FullId;
		long long Duration;
		uint64_t Count;

		What(std::wstring const &Id, std::wstring const &FullId)
			: Id(Id)
			, FullId(FullId)
			, Duration(0)
			, Count(0)
		{
		}
	};

	typedef std::unordered_map<std::wstring, What> TItems;

	static long& Ref()
	{
		static long _Ref = 0;
		return _Ref;
	}

	static TItems& Items()
	{
		static TItems _Items;
		return _Items;
	}

	static std::vector<std::wstring>& Stack()
	{
		static std::vector<std::wstring> _Stack;
		return _Stack;
	}

	static std::wstring GetPrev()
	{
		std::wstring Id;
		auto& s = Stack();
		if (!s.empty())
			Id = s.back();

		return Id;
	}

	static void PopPrev()
	{
		auto& s = Stack();
		if (!s.empty())
			s.pop_back();
	}

	static void SetPrev(std::wstring const &Id)
	{
		auto& s = Stack();
		s.push_back(Id);
	}

	static std::wstring MakeId(std::wstring const &Id)
	{
		std::wstring w;
		auto& s = Stack();
		for (auto const &i : s)
		{
			if (!w.empty())
				w.append(L"/");

			w.append(i);
		}

		if (!w.empty())
			w.append(L"/");

		w.append(Id);

		return w;
	}
	
	std::wstring m_Id;
	LARGE_INTEGER m_Started;
};

#ifdef PERFTIMER_ENABLED
#define PERFTIMER_SCOPE(Name)     PerformanceTimer __PerfTimer__ ## __LINE__(L ## Name)
#define PERFTIMER_SCOPEW(Name)    PerformanceTimer __PerfTimer__ ## __LINE__(Name)
#else
#define PERFTIMER_SCOPE(Name)     (void)0
#define PERFTIMER_SCOPEW(Name)    (void)0
#endif

