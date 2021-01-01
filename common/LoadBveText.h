#pragma once
#ifndef LOAD_BVE_TEXT_INCLUDED
#define LOAD_BVE_TEXT_INCLUDED
#include <vector>
#include <locale>
#include <string>
#include <algorithm>

//「;」もしくは「#」から始まるコメントを削除する。
template <typename T>void cleanUpBveStr(std::basic_string<T>* StrIn, const std::locale& _loc = std::locale())
{
	if (!StrIn->empty())
	{
		size_t comment;
		const T temp[] = { std::use_facet<std::ctype<T>>(_loc).widen(';'),std::use_facet<std::ctype<T>>(_loc).widen('#') };
		comment = StrIn->find_first_of(temp);
		if (comment != std::basic_string<T>::npos)StrIn->erase(StrIn->cbegin() + comment, StrIn->cend());
	}
}

template <>inline void cleanUpBveStr<char>(std::basic_string<char>* StrIn, const std::locale& _loc)
{
	if (!StrIn->empty())
	{
		size_t comment;
		comment = StrIn->find_first_of(";#");
		if (comment != std::basic_string<char>::npos)StrIn->erase(StrIn->cbegin() + comment, StrIn->cend());
	}
}

//文字列の初めと終わりのスペースやタブを削除する。
template<typename T>void eraseSpace(std::basic_string<T>* _src, const std::locale& _loc = std::locale())
{
	if (!_src->empty())
	{
		while (std::isspace(_src->front(), _loc))_src->erase(_src->cbegin());
		while (std::isspace(_src->back(), _loc))_src->pop_back();
	}
}

template <typename T>size_t splitSymbol(const T& symbol, const std::basic_string<T>& _src, std::basic_string<T>* _left, std::basic_string<T>* _right, const std::locale& _loc = std::locale())
{
	size_t pos = std::basic_string<T>::npos;
	if (!_src.empty())
	{
		pos = _src.find_first_of(symbol);
		if (pos != std::basic_string<T>::npos)
		{
			*_left = _src.substr(0, pos);
			*_right = _src.substr(pos + 1);
			eraseSpace(_left, _loc);
			eraseSpace(_right, _loc);
		}
	}
	return pos;
}

//csvの行を分割する。
template <typename T>void splitCsvColumn(const std::basic_string<T>& loadline, std::vector<std::basic_string<T>>* columun, const std::locale& _loc = std::locale())
{
	if (!loadline.empty())
	{
		size_t begin = 0, comma = 0;
		T commaWiden = std::use_facet<std::ctype<T>>(_loc).widen(',');
		while (comma < loadline.length())
		{
			if (loadline.at(comma) == commaWiden)
			{
				columun->emplace_back(loadline.substr(begin, comma - begin));
				begin = comma + 1;
			}
			comma++;
		}
		if (begin < loadline.length()) columun->emplace_back(loadline.substr(begin));
		for (auto& a : *columun)eraseSpace(&a, _loc);
	}
}

template <>inline void splitCsvColumn<char>(const std::basic_string<char>& loadline, std::vector<std::basic_string<char>>* columun, const std::locale& _loc)
{
	if (!loadline.empty())
	{
		size_t begin = 0, comma = 0;
		while (comma < loadline.length())
		{
			if (loadline.at(comma) == ',')
			{
				columun->emplace_back(loadline.substr(begin, comma - begin));
				begin = comma + 1;
			}
			comma++;
		}
		if (begin < loadline.length()) columun->emplace_back(loadline.substr(begin));
		for (auto& a : *columun)eraseSpace(&a, _loc);
	}
}

//大文字と小文字を区別しない文字比較(等しいときにtrue)
template <typename T>bool icasecmp(const std::basic_string<T>& x, const std::basic_string<T>& y, const std::locale& _loc = std::locale())
{
	if (!x.empty() && !y.empty())
	{
		return equal(x.cbegin(), x.cend(), y.cbegin(), y.cend(),
			[&](typename std::basic_string<T>::value_type x1, typename std::basic_string<T>::value_type y1)
			{ return std::toupper<T>(x1, _loc) == std::toupper<T>(y1, _loc); });
	}
	else return false;
}

template <typename T>bool icasecmp(const std::basic_string<T>& x, const T* y, const std::locale& _loc = std::locale())
{
	std::basic_string_view<T> z = (y);
	if (!x.empty() && !z.empty())
	{
		return equal(x.cbegin(), x.cend(), z.cbegin(), z.cend(),
			[&](typename std::basic_string<T>::value_type x1, typename std::basic_string_view<T>::value_type z1)
			{ return std::toupper<T>(x1, _loc) == std::toupper<T>(z1, _loc); });
	}
	else return false;
}


#endif // !LOAD_BVE_TEXT_INCLUDED

