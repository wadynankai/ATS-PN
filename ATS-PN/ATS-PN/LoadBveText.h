#pragma once
#ifndef LOAD_BVE_TEXT_INCLUDED
#define LOAD_BVE_TEXT_INCLUDED
#include"pch.h"
#include <vector>
#include <string>
#include <algorithm>

void cleanUpBveStr(std::string* StrIn);//�R�����g��X�y�[�X����������
void cleanUpBveStr(std::wstring* StrIn);//�R�����g��X�y�[�X����������

void splitCsvColumn(const std::string& loadline, std::vector<std::string>* columun);//csv�̍s�𕪊�����B
void splitCsvColumn(const std::wstring& loadline, std::vector<std::wstring>* columun);//csv�̍s�𕪊�����B

//�啶���Ə���������ʂ��Ȃ�������r(�������Ƃ���true)
template <typename T> bool icasecmp(const T& l, const T& r)
{
	return l.size() == r.size()
		&& equal(l.cbegin(), l.cend(), r.cbegin(),
			[](typename T::value_type l1, typename T::value_type r1)
			{ return towupper(l1) == towupper(r1); });
}

#endif // !LOAD_BVE_TEXT_INCLUDED

