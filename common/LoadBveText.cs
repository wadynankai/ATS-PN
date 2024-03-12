using System;
using System.CodeDom.Compiler;

namespace AtsPlugin
{
	internal class LoadBveText
	{
		//「;」もしくは「#」から始まるコメントを削除する。
		public static string cleanUpBveStr(string StrIn)
		{
			if (!string.IsNullOrEmpty(StrIn))
			{
				Char[] symbols = new char[2] { '#', ';' };
				Int32 comment = StrIn.IndexOfAny(symbols);
				if (comment != -1)
				{
					return StrIn.Remove(comment);
                }
                else return StrIn;
            }
			else return StrIn;
		}

		//文字列の初めと終わりのスペースやタブを削除する。
		public static string eraseSpace(string _src)
		{
			if (!string.IsNullOrEmpty(_src))
			{
				while (Char.IsWhiteSpace(_src, 0))
				{
					_src.Remove(0, 1);
					if (!string.IsNullOrEmpty(_src)) break;
				}
			}
			if (!string.IsNullOrEmpty(_src))
			{
				while (Char.IsWhiteSpace(_src[_src.Length - 1]))
				{
					_src.Remove(_src.Length - 1);
					if (!string.IsNullOrEmpty(_src)) break;
				}
			}
			return _src;
		}

		/*template < typename T > size_t splitSymbol(const T& symbol, const std::basic_string<T>& _src, std::basic_string<T>& _left, std::basic_string<T>& _right, const std::locale& _loc = {})
		{
			size_t pos = std::basic_string < T >::npos;
			if (!_src.empty())
			{
				pos = _src.find_first_of(symbol);
				if (pos != std::basic_string < T >::npos)
				{
					_left = _src.substr(0, pos);
					_right = _src.substr(pos + 1);
					eraseSpace(_left, _loc);
					eraseSpace(_right, _loc);
				}
			}
			return pos;
		}*/

		//csvの行を分割する。
		/*template < typename T > void splitCsvColumn(const std::basic_string<T>& loadline, std::vector<std::basic_string<T>>& columun, const std::locale& _loc = {})
		{
			if (!loadline.empty())
			{
				size_t begin = 0, comma = 0;
				T comma_symbol;
				if constexpr(std::is_same_v<T, char>)comma_symbol = ',';
				else if constexpr(std::is_same_v<T, wchar_t>)comma_symbol = L',';
				else if constexpr(std::is_same_v<T, char8_t>)comma_symbol = u8',';
				else if constexpr(std::is_same_v<T, char16_t>)comma_symbol = u',';
				else if constexpr(std::is_same_v<T, char32_t>)comma_symbol = U',';
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
				for (auto & a : columun)
				{
					eraseSpace(a, _loc);
				}
			}
		}*/

		//大文字と小文字を区別しない文字比較(等しいときにtrue)
		/*
		bool icasecmp(const std::basic_string<T>& x, const std::basic_string<T>& y, const std::locale& _loc = {})
		{
			if (!x.empty() && !y.empty())
			{
				return std::equal(x.cbegin(), x.cend(), y.cbegin(), y.cend(),
					[&](typename std::basic_string < T >::value_type x1, typename std::basic_string < T >::value_type y1)
					{ return std::toupper<T>(x1, _loc) == std::toupper<T>(y1, _loc); });
			}
			else return false;
		}

		template < typename T > _NODISCARD bool icasecmp(const std::basic_string<T>& x, const T* y, const std::locale& _loc = {})
		{
			std::basic_string_view<T> z = (y);
			if (!x.empty() && !z.empty())
			{
				return std::equal(x.cbegin(), x.cend(), z.cbegin(), z.cend(),
					[&](typename std::basic_string < T >::value_type x1, typename std::basic_string_view < T >::value_type z1)
					{ return std::toupper<T>(x1, _loc) == std::toupper<T>(z1, _loc); });
			}
			else return false;
		}
			}*/
	}
}
