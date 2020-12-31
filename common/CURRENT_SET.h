#ifndef CURRENT_SET_INCLUDED
#define CURRENT_SET_INCLUDED
#include <string>
#include <iterator>
#include <charconv>
#include <utility>
#include <vector>
#include <fstream>
#include "LoadBveText.h"

using CURRENT_SET = std::pair<float, float>;
using DISTANCE_SET = std::pair<float, double>;

// �����ɕ��ׂ�(<)
template <typename T> constexpr bool PairLesser(const T& _left, const T& _right)noexcept
{
	return _left.first < _right.first;
}
//�~���ɕ��ׂ�(>)
template <typename T> constexpr bool PairGreater(const T& _left, const T& _right)noexcept
{
	return _left.first > _right.first;
}


//csv�ǂݍ���
template <typename T, typename X, typename Y> void makeTableFromCsv(
	const T& name,//csv�t�@�C���̐�΃p�X
	std::vector <std::pair<X, Y>>* table ,//�e�[�u���ƂȂ�vector�ւ̃|�C���^
	size_t row = 1//csv�t�@�C���œǂݍ��ޗ�
)
{
	table->clear();
	std::ifstream csv(name);
	if (csv.is_open())
	{
		csv.imbue(std::locale("ja-JP"));
		std::string loadline;
		std::getline(csv, loadline);
		while (!csv.eof())
		{
			std::vector<std::string> columun;
			std::getline(csv, loadline);
			cleanUpBveStr(&loadline, csv.getloc());
			if (!loadline.empty())
			{
				splitCsvColumn(loadline, &columun, csv.getloc());
				if (!columun.at(row).empty())
				{
					X speed{};
					Y current{};
					if (std::from_chars(columun.at(0).data(), columun.at(0).data() + columun.at(0).length(), speed).ec == std::errc{}
						&& std::from_chars(columun.at(row).data(), columun.at(row).data() + columun.at(row).length(), current).ec == std::errc{})
					{
						table->emplace_back(speed, current);
					}
				}
			}
		}
		csv.close();
	}
	if (!table->empty())
	{
		std::stable_sort(table->begin(), table->end(), PairLesser<std::pair<X, Y>>);
		table->shrink_to_fit();
	}
}

//csv�ǂݍ���
template <typename T, typename X, typename Y> void makeTableFromCsv(
	const T* name,//csv�t�@�C���̐�΃p�X
	std::vector <std::pair<X, Y>>* table,//�e�[�u���ƂȂ�vector�ւ̃|�C���^
	size_t row = 1//csv�t�@�C���œǂݍ��ޗ�
)
{
	makeTableFromCsv<std::basic_string<T>, X, Y>(static_cast<std::basic_string<T>>(name), table, row);
}

//2�_�Ԃ�ʂ钼���̌X��
template <typename X, typename Y> _NODISCARD inline constexpr Y slope(const std::pair<X, Y>& p1, const std::pair<X, Y>& p2)noexcept
{
	return (p2.second - p1.second) / (static_cast<Y>(p2.first) - static_cast<Y>(p1.first));
}

//2�_�Ԃ�ʂ钼����y�ؕ�
template <typename X, typename Y> _NODISCARD inline constexpr Y intercept(const std::pair<X, Y>& p1, const std::pair<X, Y>& p2)noexcept
{
	return -slope(p1, p2) * static_cast<Y>(p1.first) + p1.second;
}

//2�_�Ԃ�ʂ钼���̕�����(first:�X���Csecond:y�ؕ�)
template <typename X, typename Y> _NODISCARD inline constexpr const std::pair<Y, Y> linear(const std::pair<X, Y>& p1, const std::pair<X, Y>& p2)noexcept
{
	return { slope(p1,p2),intercept(p1,p2) };
}


//���`���
template <typename X, typename Y> _NODISCARD inline constexpr Y interpolation(
	const X value,//x���W�i���x�j
	const std::vector <std::pair<X, Y>>& table//�e�[�u���ƂȂ�vector
)noexcept
{
	if (table.size() > 1)
	{
		size_t j = 0;
		for(const auto& a: table)
		{
			if (a.first >= value)break;
			j++;
		}
		if (j > 0 && j < table.size())
		{
			if (table.at(j).first != table.at(j - 1).first)
			{
				return slope(table.at(j), table.at(j - 1)) * (static_cast<Y>(value) - static_cast<Y>(table.at(j).first)) + table.at(j).second;
			}
			else return table.at(j).second;
		}
		else if (j == 0)
		{
			if (table.at(j + 1).first != table.at(j).first)
			{
				return slope(table.at(j + 1), table.at(j)) * (static_cast<Y>(value) - static_cast<Y>(table.at(j + 1).first)) + table.at(j + 1).second;
			}
			else return table.at(j).second;
		}
		else
		{
			if (table.at(table.size() - 1).first != table.at(table.size() - 2).first)
			{
				return slope(table.at(table.size() - 1), table.at(table.size() - 2)) * (static_cast<Y>(value) - static_cast<Y>(table.at(table.size() - 1).first)) + table.at(table.size() - 1).second;
			}
			else return table.at(table.size() - 1).second;
		}
	}
	else if (table.size() == 1)return table.at(0).second;
	else return value;
}//���`���


#endif // !CURRENT_SET_INCLUDED