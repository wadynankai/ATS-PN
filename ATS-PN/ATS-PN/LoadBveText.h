#ifndef LOAD_BVE_TEXT_INCLUDED
#define LOAD_BVE_TEXT_INCLUDED
#include"pch.h"
#include <vector>
#include <string>
#include <algorithm>
//コメントやスペースを消去する
void cleanUpBveStr(std::string* StrIn);
void cleanUpBveStr(std::wstring* StrIn);
//csvの行を分割する。
void splitCsvColumn(const std::string& loadline, std::vector<std::string>* columun);
void splitCsvColumn(const std::wstring& loadline, std::vector<std::wstring>* columun);
//大文字と小文字を区別しない文字比較(等しいときにtrue)
//bool icasecmp(const std::string& l, const std::string& r);
//bool icasecmp(const std::wstring& l, const std::wstring& r);
template <typename T> bool icasecmp(const T& l, const T& r)
{
	return l.size() == r.size()
		&& equal(l.cbegin(), l.cend(), r.cbegin(),
			[](typename T::value_type l1, typename T::value_type r1)
			{ return towupper(l1) == towupper(r1); });
}

#endif // !LOAD_BVE_TEXT_INCLUDED

