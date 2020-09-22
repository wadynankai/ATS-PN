#include"pch.h"
#include"LoadBveText.h"
void cleanUpBveStr(std::string* StrIn)
{
	size_t comment;
	comment = StrIn->find_first_of(";#");
	if (comment != std::string::npos)StrIn->erase(StrIn->cbegin() + comment, StrIn->cend());
	for (size_t i = 0; i < StrIn->length(); ++i)//�폜�𔺂����߁C�͈�for���͎g��Ȃ�
	{
		if (StrIn->at(i) == (' '))StrIn->erase(StrIn->cbegin() + i);
	}
}
void cleanUpBveStr(std::wstring* StrIn)
{
	size_t comment;
	comment = StrIn->find_first_of(L";#");
	if (comment != std::wstring::npos)StrIn->erase(StrIn->cbegin() + comment, StrIn->cend());
	for (size_t i = 0; i < StrIn->length(); ++i)//�폜�𔺂����߁C�͈�for���͎g��Ȃ�
	{
		if (StrIn->at(i) == (L' '))StrIn->erase(StrIn->cbegin() + i);
	}

}

//csv�̍s�𕪊�����B
void splitCsvColumn(const std::string& loadline, std::vector<std::string>* columun)
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
}
void splitCsvColumn(const std::wstring& loadline, std::vector<std::wstring>* columun)
{
	size_t begin = 0, comma = 0;
	while (comma < loadline.length())
	{
		if (loadline.at(comma) == L',')
		{
			columun->emplace_back(loadline.substr(begin, comma - begin));
			begin = comma + 1;
		}
		comma++;
	}
	if (begin < loadline.length()) columun->emplace_back(loadline.substr(begin));
}