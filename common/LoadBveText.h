#pragma once
#ifndef LOAD_BVE_TEXT_INCLUDED
#define LOAD_BVE_TEXT_INCLUDED
#include <vector>
#include <locale>
#include <string>
#include <algorithm>

//�u;�v�������́u#�v����n�܂�R�����g���폜����B
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

//������̏��߂ƏI���̃X�y�[�X��^�u���폜����B
template<typename T>void eraseSpace(std::basic_string<T>* _src, const std::locale& _loc = std::locale())
{
	if (!_src->empty())
	{
		while (std::isspace(_src->front(), _loc))_src->erase(_src->cbegin());
		while (std::isspace(_src->back(), _loc))_src->pop_back();
	}
}

template <typename T>void splitSymbol(const char& symbol,const std::basic_string<T>& _src, std::basic_string<T>* _left, std::basic_string<T>* _right, const std::locale& _loc = std::locale())
{
	if (!_src.empty())
	{
		size_t pos;
		pos = _src.find_first_of(std::use_facet<std::ctype<T>>(_loc).widen(symbol));
		*_left = _src.substr(0, pos);
		*_right = _src.substr(pos + 1);
		eraseSpace(_left, _loc);
		eraseSpace(_right, _loc);
	}
}

//csv�̍s�𕪊�����B
template <typename T>void splitCsvColumn(const std::basic_string<T>& loadline, std::vector<std::basic_string<T>>* columun, const std::locale& _loc = std::locale())
{
	if (!loadline.empty())
	{
		size_t begin = 0, comma = 0;
		while (comma < loadline.length())
		{
			if (loadline.at(comma) == std::use_facet<std::ctype<T>>(_loc).widen(','))
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

//�啶���Ə���������ʂ��Ȃ�������r(�������Ƃ���true)
template <typename T>bool icasecmp(const std::basic_string<T>& x, const std::basic_string<T>& y, const std::locale& _loc=std::locale())
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

