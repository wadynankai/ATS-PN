#ifndef LOAD_BVE_TEXT_INCLUDED
#define LOAD_BVE_TEXT_INCLUDED

#include<vector>
#include<string>
#include<locale>
#include <type_traits>

//「;」もしくは「#」から始まるコメントを削除する。
template <typename T>void cleanUpBveStr(std::basic_string<T>& StrIn, const std::locale& _loc = {})
{
	if (!StrIn.empty())
	{
		size_t comment;
		if constexpr (std::is_same_v<T, char>)comment = StrIn.find_first_of(";#\0");
		else if constexpr (std::is_same_v<T, wchar_t>)comment = StrIn.find_first_of(L";#\0");
		else if constexpr (std::is_same_v <T, char8_t>)comment = StrIn.find_first_of(u8";#\0");
		else if constexpr (std::is_same_v <T, char16_t>)comment = StrIn.find_first_of(u";#\0");
		else if constexpr (std::is_same_v <T, char32_t>)comment = StrIn.find_first_of(U";#\0");
		if (comment != std::basic_string<T>::npos)StrIn.erase(StrIn.cbegin() + comment, StrIn.cend());
	}
}

//文字列の初めと終わりのスペースやタブを削除する。
template<typename T>void eraseSpace(std::basic_string<T>& _src, const std::locale& _loc = {})
{
	if (!_src.empty())
	{
		while (std::isspace(_src.front(), _loc))
		{
			_src.erase(_src.cbegin());
			if (_src.empty())break;
		}
	}
	if (!_src.empty())
	{
		while (std::isspace(_src.back(), _loc))
		{
			_src.pop_back();
			if (_src.empty())break;
		}
	}
}

template <typename T> size_t splitSymbol(const T& symbol, const std::basic_string<T>& _src, std::basic_string<T>& _left, std::basic_string<T>& _right, const std::locale& _loc = {})
{
	size_t pos = std::basic_string<T>::npos;
	if (!_src.empty())
	{
		pos = _src.find_first_of(symbol);
		if (pos != std::basic_string<T>::npos)
		{
			_left = _src.substr(0, pos);
			_right = _src.substr(pos + 1);
			eraseSpace(_left, _loc);
			eraseSpace(_right, _loc);
		}
	}
	return pos;
}

//csvの行を分割する。
template <typename T>void splitCsvColumn(const std::basic_string<T>& loadline, std::vector<std::basic_string<T>>& columun, const std::locale& _loc = {})
{
	if (!loadline.empty())
	{
		size_t begin = 0, comma = 0;
		T comma_symbol;
		if constexpr (std::is_same_v<T, char>)comma_symbol = ',';
		else if constexpr (std::is_same_v <T, wchar_t>)comma_symbol = L',';
		else if constexpr (std::is_same_v<T, char8_t>)comma_symbol = u8',';
		else if constexpr (std::is_same_v<T, char16_t>)comma_symbol = u',';
		else if constexpr (std::is_same_v<T, char32_t>)comma_symbol = U',';
		columun.clear();
		while (comma < loadline.length())
		{
			if (loadline.at(comma) == comma_symbol)
			{
				columun.emplace_back(loadline.substr(begin, comma - begin));
				begin = comma + 1;
			}
			comma++;
		}
		if (begin < loadline.length()) columun.emplace_back(loadline.substr(begin));
		for (auto& a : columun)
		{
			eraseSpace(a, _loc);
		}
	}
}

//大文字と小文字を区別しない文字比較(等しいときにtrue)
template <typename T>_NODISCARD bool icasecmp(const std::basic_string<T>& x, const std::basic_string<T>& y, const std::locale& _loc = {})
{
	if (!x.empty() && !y.empty())
	{
		return std::equal(x.cbegin(), x.cend(), y.cbegin(), y.cend(),
			[&](typename std::basic_string<T>::value_type x1, typename std::basic_string<T>::value_type y1)
			{ return std::toupper<T>(x1, _loc) == std::toupper<T>(y1, _loc); });
	}
	else return false;
}

template <typename T>_NODISCARD bool icasecmp(const std::basic_string<T>& x, const T* y, const std::locale& _loc = {})
{
	std::basic_string_view<T> z = (y);
	if (!x.empty() && !z.empty())
	{
		return std::equal(x.cbegin(), x.cend(), z.cbegin(), z.cend(),
			[&](typename std::basic_string<T>::value_type x1, typename std::basic_string_view<T>::value_type z1)
			{ return std::toupper<T>(x1, _loc) == std::toupper<T>(z1, _loc); });
	}
	else return false;
}

template <typename T> _NODISCARD inline size_t stoz(const T& _Str, size_t* _Idx = nullptr, int _Base = 10)//文字列をsize_tに変換する。
{
#ifndef _WIN64
	return std::stoul(_Str, _Idx, _Base);//32ビット版
#else
	return std::stoull(_Str, _Idx, _Base);//64ビット版
#endif
}

#endif // !LOAD_BVE_TEXT_INCLUDED

