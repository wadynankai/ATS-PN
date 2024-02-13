#ifndef CURRENT_SET_INCLUDED
#define CURRENT_SET_INCLUDED

#include <algorithm>
#include <utility>
#include <vector>
#include <fstream>
#include <string>
#include <locale>
#include "LoadBveText.h"

using CURRENT_SET = std::pair<float, float>;
using DISTANCE_SET = std::pair<float, double>;

// 昇順に並べる(<)
template <typename T> struct PairLesser
{
	[[nodiscard]] inline constexpr bool operator ()(const T& _left, const T& _right)noexcept
	{
		return _left.first < _right.first;
	}
};

//降順に並べる(>)
template <typename T> struct PairGreater
{
	[[nodiscard]] inline constexpr bool operator ()(const T& _left, const T& _right)noexcept
	{
		return _left.first > _right.first;
	}
};


//csv読み込み
template <typename T, typename X, typename Y> void makeTableFromCsv(
	const T& name,//csvファイルの絶対パス
	std::vector <std::pair<X, Y>>& table ,//テーブルとなるvector
	const size_t row = 1,//csvファイルで読み込む列
	const std::locale& loc ={}
)
{
	table.clear();
	std::wifstream csv(name);
	if (csv.is_open())
	{
		csv.imbue(loc);
		std::wstring loadline;
		std::getline(csv, loadline);
		while (!csv.eof())
		{
			std::vector<std::wstring> columun;
			std::getline(csv, loadline);
			cleanUpBveStr(loadline, csv.getloc());
			if (!loadline.empty())
			{
				splitCsvColumn(loadline, columun, csv.getloc());
				if (!columun.at(row).empty())
				{
					X speed{};
					Y current{};
					if constexpr (std::is_same_v<X, int>)speed = std::stoi(columun.at(0));
					else if constexpr (std::is_same_v<X, long>)speed = std::stol(columun.at(0));
					else if constexpr (std::is_same_v<X, long long>)speed = std::stoll(columun.at(0));
					else if constexpr (std::is_same_v<X, unsigned long>)speed = std::stoul(columun.at(0));
					else if constexpr (std::is_same_v<X, unsigned long long>)speed = std::stoull(columun.at(0));
					else if constexpr (std::is_same_v<X, float>)speed = std::stof(columun.at(0));
					else if constexpr (std::is_same_v<X, double>)speed = std::stod(columun.at(0));
					else if constexpr (std::is_same_v<X, long double>)speed = std::stold(columun.at(0));

					if constexpr (std::is_same_v<Y, int>)current = std::stoi(columun.at(row));
					else if constexpr (std::is_same_v<Y, long>)current = std::stol(columun.at(row));
					else if constexpr (std::is_same_v<Y, long long>)current = std::stoll(columun.at(row));
					else if constexpr (std::is_same_v<Y, unsigned long>)current = std::stoul(columun.at(row));
					else if constexpr (std::is_same_v<Y, unsigned long long>)current = std::stoull(columun.at(row));
					else if constexpr (std::is_same_v<Y, float>)current = std::stof(columun.at(row));
					else if constexpr (std::is_same_v<Y, double>)current = std::stod(columun.at(row));
					else if constexpr (std::is_same_v<Y, long double>)current = std::stold(columun.at(row));


					table.emplace_back(speed, current);
				}
			}
		}
		csv.close();
	}
	if (!table.empty())
	{
		std::stable_sort(table.begin(), table.end(), PairLesser<std::pair<X, Y>>());
		table.shrink_to_fit();
	}
}

//csv読み込み
template <typename T, typename X, typename Y> void makeTableFromCsv(
	const T* name,//csvファイルの絶対パス
	std::vector <std::pair<X, Y>>& table,//テーブルとなるvectorへのポインタ
	const size_t row = 1,//csvファイルで読み込む列
	const std::locale& loc = {}
)
{
	makeTableFromCsv<std::basic_string<T>, X, Y>(static_cast<std::basic_string<T>>(name), table, row, loc);
}

//csv読み込み(インデックスリスト使用)
template <typename T,typename X, typename Y, typename U> void makeTableFromCsv(
	const T& name,//csvファイルの絶対パス
	std::vector<std::vector <std::pair<X, Y>>>& table,//テーブルとなるvector
	const U& index_list = {1},//csvファイルで読み込む列のリスト
	const std::locale& loc = {}
)
{
	table.clear();
	table.resize(index_list.size());
	std::wifstream csv(name);
	if (csv.is_open())
	{
		csv.imbue(loc);
		std::wstring loadline;
		std::getline(csv, loadline);
		while (!csv.eof())
		{
			std::vector<std::wstring> columun;
			std::getline(csv, loadline);
			cleanUpBveStr(loadline, csv.getloc());
			if (!loadline.empty())
			{
				splitCsvColumn(loadline, columun, csv.getloc());
				size_t size = columun.size();
				for (auto itr = table.begin(); auto& a : index_list)
				{
					if (size > a)
					{
						if (!columun.at(a).empty())
						{
							X speed;
							Y current;
							if constexpr (std::is_same_v<X, int>)speed = std::stoi(columun.at(0));
							else if constexpr (std::is_same_v<X, long>)speed = std::stol(columun.at(0));
							else if constexpr (std::is_same_v<X, long long>)speed = std::stoll(columun.at(0));
							else if constexpr (std::is_same_v<X, unsigned long>)speed = std::stoul(columun.at(0));
							else if constexpr (std::is_same_v<X, unsigned long long>)speed = std::stoull(columun.at(0));
							else if constexpr (std::is_same_v<X, float>)speed = std::stof(columun.at(0));
							else if constexpr (std::is_same_v<X, double>)speed = std::stod(columun.at(0));
							else if constexpr (std::is_same_v<X, long double>)speed = std::stold(columun.at(0));

							if constexpr (std::is_same_v<Y, int>)current = std::stoi(columun.at(a));
							else if constexpr (std::is_same_v<Y, long>)current = std::stol(columun.at(a));
							else if constexpr (std::is_same_v<Y, long long>)current = std::stoll(columun.at(a));
							else if constexpr (std::is_same_v<Y, unsigned long>)current = std::stoul(columun.at(a));
							else if constexpr (std::is_same_v<Y, unsigned long long>)current = std::stoull(columun.at(a));
							else if constexpr (std::is_same_v<Y, float>)current = std::stof(columun.at(a));
							else if constexpr (std::is_same_v<Y, double>)current = std::stod(columun.at(a));
							else if constexpr (std::is_same_v<Y, long double>)current = std::stold(columun.at(a));

							itr->emplace_back(speed, current);
						}
					}
					++itr;
				}
			}
		}
	}
	csv.close();
	for (auto& a : table)
	{
		if (!a.empty())
		{
			std::stable_sort(a.begin(), a.end(), PairLesser<std::pair<X, Y>>());
			a.shrink_to_fit();
		}
	}
	table.shrink_to_fit();
}

//csv読み込み(インデックスリスト使用)
template <typename T, typename X, typename Y, typename U> void makeTableFromCsv(
	const T* name,//csvファイルの絶対パス
	std::vector<std::vector <std::pair<X, Y>>>& table,//テーブルとなるvector
	const U& index_list = { 1 },//csvファイルで読み込む列のリスト
	const std::locale& loc = {}
)
{
	makeTableFromCsv<std::basic_string<T>, X, Y, U>(static_cast<std::basic_string<T>>(name), table, index_list, loc);
}

//2点間を通る直線の傾き
template <typename X, typename Y> [[nodiscard]] inline constexpr Y slope(const std::pair<X, Y>& p1, const std::pair<X, Y>& p2)noexcept
{
	return (p2.second - p1.second) / (static_cast<Y>(p2.first) - static_cast<Y>(p1.first));
}

//2点間を通る直線の傾き(逆関数)
template <typename X, typename Y> [[nodiscard]] inline constexpr X slopeInv(const std::pair<X, Y>& p1, const std::pair<X, Y>& p2)noexcept
{
	return (p2.first - p1.first) / (static_cast<X>(p2.second) - static_cast<X>(p1.second));
}

//2点間を通る直線の傾き
template <typename X, typename Y> [[nodiscard]] inline constexpr Y slope(const X& x1, const Y& y1, const X& x2, const Y& y2)noexcept
{
	return (y2 - y1) / (static_cast<Y>(x2) - static_cast<Y>(x1));
}

//2点間を通る直線のy切片
template <typename X, typename Y> [[nodiscard]] inline constexpr Y intercept(const std::pair<X, Y>& p1, const std::pair<X, Y>& p2)noexcept
{
	return -slope(p1, p2) * static_cast<Y>(p1.first) + p1.second;
}

//2点間を通る直線のy切片
template <typename X, typename Y> [[nodiscard]] inline constexpr Y intercept(const X& x1, const Y& y1, const X& x2, const Y& y2)noexcept
{
	return -slope(x1, y1, x2, y2) * static_cast<Y>(x1) + y1;
}

//2点間を通る直線の方程式(first:傾き，second:y切片)
template <typename X, typename Y> [[nodiscard]] inline constexpr const std::pair<Y, Y> linear(const std::pair<X, Y>& p1, const std::pair<X, Y>& p2)noexcept
{
	return { slope(p1,p2),intercept(p1,p2) };
}

//2点間を通る直線の方程式(first:傾き，second:y切片)
template <typename X, typename Y> [[nodiscard]] inline constexpr const std::pair<Y, Y> linear(const X& x1, const Y& y1, const X& x2, const Y& y2)noexcept
{
	return { slope(x1, y1, x2, y2), intercept(x1, y1, x2, y2) };
}



//線形補間
template <typename X, typename Y> [[nodiscard]] inline constexpr Y interpolation(
	const X value,//x座標（速度）
	const std::vector <std::pair<X, Y>>& table//テーブルとなるvector
)noexcept
{
	if (table.size() > 1)
	{
		size_t j = 0;
		for (const auto& a : table)
		{
			if (a.first >= value)break;
			j++;
		}
		if (j > 0 && j < table.size())
		{
			if (table.at(j).first != table.at(j - 1).first)
			{
				return slope(table.at(j), table.at(j - 1)) * static_cast<Y>(value - table.at(j).first) + table.at(j).second;
			}
			else return table.at(j).second;
		}
		else if (j == 0)
		{
			if (table.at(j + 1).first != table.at(j).first)
			{
				return slope(table.at(j + 1), table.at(j)) * static_cast<Y>(value - table.at(j + 1).first) + table.at(j + 1).second;
			}
			else return table.at(j).second;
		}
		else
		{
			if (table.at(table.size() - 1).first != table.at(table.size() - 2).first)
			{
				return slope(table.at(table.size() - 1), table.at(table.size() - 2)) * static_cast<Y>(value - table.at(table.size() - 1).first) + table.at(table.size() - 1).second;
			}
			else return table.at(table.size() - 1).second;
		}
	}
	else if (table.size() == 1)return table.at(0).second;
	else return static_cast<Y>(value);
}//線形補間

//線形補間(逆関数)
template <typename X, typename Y> [[nodiscard]] inline constexpr X interpolationInv(
	const Y value,//Y座標（距離？）
	const std::vector <std::pair<X, Y>>& table//テーブルとなるvector
)noexcept
{
	if (table.size() > 1)
	{
		size_t j = 0;
		for (const auto& a : table)
		{
			if (a.second >= value)break;
			j++;
		}
		if (j > 0 && j < table.size())
		{
			if (table.at(j).second != table.at(j - 1).second)
			{
				return slopeInv(table.at(j), table.at(j - 1)) * static_cast<X>(value - table.at(j).second) + table.at(j).first;
			}
			else return table.at(j).first;
		}
		else if (j == 0)
		{
			if (table.at(j + 1).second != table.at(j).second)
			{
				return slopeInv(table.at(j + 1), table.at(j)) * static_cast<X>(value - table.at(j + 1).second) + table.at(j + 1).first;
			}
			else return table.at(j).first;
		}
		else
		{
			if (table.at(table.size() - 1).second != table.at(table.size() - 2).second)
			{
				return slopeInv(table.at(table.size() - 1), table.at(table.size() - 2)) * static_cast<X>(value - table.at(table.size() - 1).second) + table.at(table.size() - 1).first;
			}
			else return table.at(table.size() - 1).first;
		}
	}
	else if (table.size() == 1)return table.at(0).first;
	else return static_cast<X>(value);
}//線形補間(逆関数)

#endif // !CURRENT_SET_INCLUDED