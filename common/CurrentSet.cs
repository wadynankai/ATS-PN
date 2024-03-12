using System;
using System.IO;
using System.Collections.Generic;
#if VISUAL_BASIC
using Microsoft.VisualBasic.FileIO;
#endif
using System.Linq;
using System.Text;


namespace AtsPlugin
{

    internal struct Tuple<X, Y> : IEquatable<Tuple<X, Y>>, IComparable<Tuple<X, Y>>
        where X : IEquatable<X>, IComparable<X>, IConvertible
        where Y : IEquatable<Y>, IComparable<Y>, IConvertible
    {
		public X first { get; set; }
		public Y second { get; set; }

		public Tuple(X x, Y y)
		{
			first = x; second = y;
		}
		public override int GetHashCode()
        {
            int hashCode = 405212230;
            hashCode = hashCode * -1521134295 + EqualityComparer<X>.Default.GetHashCode(first);
            hashCode = hashCode * -1521134295 + EqualityComparer<Y>.Default.GetHashCode(second);
            return hashCode;
        }
        public bool Equals(Tuple<X, Y> _right)
        {
            return this.first.Equals(_right.first) && this.second.Equals(_right.second);
        }
		public override bool Equals(object o)
		{
			return (o is Tuple<X, Y>) && this.Equals((Tuple<X, Y>)o);
		}

		static public bool operator ==(Tuple<X, Y> _left, Tuple<X, Y> _right)
        {
            return _left.first.Equals(_right.first) && _left.second.Equals(_right.second);
        }
        static public bool operator !=(Tuple<X, Y> _left, Tuple<X, Y> _right)
        {
            return !_left.first.Equals(_right.first) || !_left.second.Equals(_right.second);
        }

        public int CompareTo(Tuple<X, Y> _right)
        {
            return this.first.CompareTo(_right.first);
        }
        static public bool operator >(Tuple<X, Y> _left, Tuple<X, Y> _right)
        {
			return _left.first.CompareTo(_right.first) > 0;
        } 
		static public bool operator >=(Tuple<X, Y> _left, Tuple<X, Y> _right)
        {
            return _left.first.CompareTo(_right.first) >= 0;
        }
        static public bool operator <(Tuple<X, Y> _left, Tuple<X, Y> _right)
        {
			return _left.first.CompareTo(_right.first) < 0;
        }
        static public bool operator <=(Tuple<X, Y> _left, Tuple<X, Y> _right)
        {
            return _left.first.CompareTo(_right.first) <= 0;
        }
        public static void makeTableFromCsv(string name,//csvファイルの絶対パス
		 List<Tuple<X,Y>> table,//テーブルとなるList
		int row = 1,//csvファイルで読み込む列
		string enc = ".UTF-8"
		)
        {
			if (File.Exists(name))
            {
                table.Clear();
#if VISUAL_BASIC
                using (TextFieldParser csvReader = new TextFieldParser(name, Encoding.GetEncoding(enc)))
                {
                    csvReader.CommentTokens = new string[] { "#" };
                    csvReader.SetDelimiters(new string[] { "," });
                    csvReader.HasFieldsEnclosedInQuotes = true;

                    //1行目はヘッダーなので飛ばす。
                    csvReader.ReadLine();
                    while (!csvReader.EndOfData)
                    {
                        string[] columun = csvReader.ReadFields();
                        if (!string.IsNullOrEmpty(columun[row]))
                        {
                            table.Add(new Tuple<X, Y>((X)Convert.ChangeType(columun[0], typeof(X)), (Y)Convert.ChangeType(columun[row], typeof(Y))));
                        }
                    }
                    csvReader.Close();
                }
#else
				using (System.IO.StreamReader csv = new System.IO.StreamReader(System.IO.File.OpenRead(name), System.Text.Encoding.GetEncoding(enc)))
                {
                    string loadline;
					loadline = csv.ReadLine();
                    while (!csv.EndOfStream)
                    {
                        string[] columun;
                        loadline = csv.ReadLine();
                        loadline = LoadBveText.cleanUpBveStr(loadline);
						if (loadline.Length > 0)
						{
							columun=loadline.Split(',');
//                            foreach (var item in columun)
 //                           {
//								LoadBveText.eraseSpace(item);
 //                           }
                            if (!string.IsNullOrEmpty(columun[row]))
                            {
                                table.Add(new Tuple<X, Y>((X)Convert.ChangeType(columun[0], typeof(X)), (Y)Convert.ChangeType(columun[row], typeof(Y))));
                            }
                        }
                    }
                }
#endif
                if (table.Count > 0)
                {
                    table.Sort();
                }
                table.TrimExcess();
            }
        }
        //csv読み込み(インデックスリスト使用)
        public static void makeTableFromCsv(
        string name,//csvファイルの絶対パス
        List<List<Tuple<X, Y>>> table,//テーブルとなるvector
        List<int> index_list,//csvファイルで読み込む列のリスト
        string enc = ".UTF-8"
    )
        {
            table.Clear();
            foreach (var a in index_list) table.Add(new List<Tuple<X, Y>>());
            if (File.Exists(name))
            {
#if VISUAL_BASIC
				using (TextFieldParser csvReader = new TextFieldParser(name, Encoding.GetEncoding(enc)))
                {
                    csvReader.CommentTokens = new string[] { "#" };
                    csvReader.SetDelimiters(new string[] { "," });
                    csvReader.HasFieldsEnclosedInQuotes = true;

                    //1行目はヘッダーなので飛ばす。
                    csvReader.ReadLine();
                    while (!csvReader.EndOfData)
                    {
                        string[] columun = csvReader.ReadFields();
                        int size = columun.Count();
                        var itr = table.GetEnumerator();
                        foreach (var a in index_list)
                        {
                            if (size > a && itr.MoveNext())
                            {
                                if (!String.IsNullOrEmpty(columun[a]))
                                {
                                    itr.Current.Add(new Tuple<X, Y>((X)Convert.ChangeType(columun[0], typeof(X)), (Y)Convert.ChangeType(columun[a], typeof(Y))));
                                }
                            }
                        }
                    }
                    csvReader.Close();
                }
#else
                using (System.IO.StreamReader csv = new System.IO.StreamReader(System.IO.File.OpenRead(name), System.Text.Encoding.GetEncoding(enc)))
                {
                    string loadline;
                    loadline = csv.ReadLine();
                    while (!csv.EndOfStream)
                    {
                        string[] columun;
                        loadline = csv.ReadLine();
                        loadline = LoadBveText.cleanUpBveStr(loadline);
                        if (loadline.Length > 0)
                        {
                            columun = loadline.Split(',');
//                            foreach (var item in columun)
//                           {
//                                LoadBveText.eraseSpace(item);
//                            }
                            int size = columun.Count();
                            var itr = table.GetEnumerator();
                            foreach (var a in index_list)
                            {
                                if (size > a && itr.MoveNext())
                                {
                                    if (!String.IsNullOrEmpty(columun[a]))
                                    {
                                        itr.Current.Add(new Tuple<X, Y>((X)Convert.ChangeType(columun[0], typeof(X)), (Y)Convert.ChangeType(columun[a], typeof(Y))));
                                    }
                                }
                            }
                        }
                    }
                }
#endif
                foreach (var a in table)
                {
                    if (a.Count > 0)
                    {
                        a.Sort();
                    }
                    a.TrimExcess();
                }

                table.TrimExcess();
            }
        }
#if NET40_OR_GREATER || NETFX_CORE
        //2点間を通る直線の傾き
        public static X slope(Tuple<X, Y> p1, Tuple<X, Y> p2)
        {
            return ((dynamic)p2.second - (dynamic)p1.second) / ((dynamic)p2.first - (dynamic)p1.first);
        }

        //2点間を通る直線の傾き(逆関数)
        public static Y slopeInv(Tuple<X, Y> p1, Tuple<X, Y> p2)
        {
            return ((dynamic)p2.first - (dynamic)p1.first) / ((dynamic)p2.second - (dynamic)p1.second);
        }

        //2点間を通る直線の傾き
        public static X slope(X x1, Y y1, X x2, Y y2)
        {
            return ((dynamic)y2 - (dynamic)y1) / ((dynamic)x2 - (dynamic)x1);
        }

        //2点間を通る直線のy切片
        public static Y intercept(Tuple<X, Y> p1, Tuple<X, Y> p2)
        {
            return -(dynamic)slope(p1, p2) * (dynamic)p1.first + (dynamic)p1.second;
        }

        //2点間を通る直線のy切片
        public static Y intercept(X x1, Y y1, X x2, Y y2)
        {
            return -(dynamic)slope(x1, y1, x2, y2) * (dynamic)x1 + (dynamic)y1;
        }

        //2点間を通る直線の方程式(first:傾き，second:y切片)
        public static Tuple<X, Y> linear(Tuple<X, Y> p1, Tuple<X, Y> p2)
        {
            return new Tuple<X, Y>(slope(p1, p2), intercept(p1, p2));
        }

        //2点間を通る直線の方程式(first:傾き，second:y切片)
        public static Tuple<X, Y> linear(X x1, Y y1, X x2, Y y2)
        {
            return new Tuple<X, Y>(slope(x1, y1, x2, y2), intercept(x1, y1, x2, y2));
        }
        //線形補間
        public static Y interpolation(
           X value,//x座標（速度）
           List<Tuple<X, Y>> table//テーブルとなるvector
       )
        {
            if (table.Count > 1)
            {
                int j = 0;
                foreach (var a in table)
                {
                    if (a.first.CompareTo(value)>=0) break;
                    j++;
                }
                if (j > 0 && j < table.Count)
                {
                    if (!table[j].first.Equals(table[j - 1].first))
                    {
                        return slope(table[j], table[j - 1]) * ((dynamic)value - (dynamic)table[j].first) + table[j].second;
                    }
                    else return table[j].second;
                }
                else if (j == 0)
                {
                    if (!table[j + 1].first.Equals(table[j].first))
                    {
                        return slope(table[j + 1], table[j]) * ((dynamic)value - (dynamic)table[j + 1].first) + table[j + 1].second;
                    }
                    else return table[j].second;
                }
                else
                {
                    if (!table[table.Count - 1].first.Equals(table[table.Count - 2].first))
                    {
                        return slope(table[table.Count - 1], table[table.Count - 2]) * ((dynamic)value - (dynamic)table[table.Count - 1].first) + table[table.Count - 1].second;
                    }
                    else return table[table.Count - 1].second;
                }
            }
            else if (table.Count == 1) return table[0].second;
            else return (Y)Convert.ChangeType(value, typeof(Y));
        }//線形補間

        //線形補間(逆関数)
        public static X interpolationInv(
        Y value,//Y座標（距離？）
        List<Tuple<X, Y>> table//テーブルとなるvector
        )
        {
            if (table.Count > 1)
            {
                int j = 0;
                foreach (var a in table)
                {
                    if (a.second.CompareTo(value)>=0) break;
                    j++;
                }
                if (j > 0 && j < table.Count)
                {
                    if (!table[j].second.Equals(table[j - 1].second))
                    {
                        return slopeInv(table[j], table[j - 1]) * ((dynamic)value - (dynamic)table[j].second) + table[j].first;
                    }
                    else return table[j].first;
                }
                else if (j == 0)
                {
                    if (!table[j + 1].second.Equals(table[j].second))
                    {
                        return slopeInv(table[j + 1], table[j]) * ((dynamic)value - (dynamic)table[j + 1].second) + table[j + 1].first;
                    }
                    else return table[j].first;
                }
                else
                {
                    if (!table[table.Count - 1].second.Equals(table[table.Count - 2].second))
                    {
                        return slopeInv(table[table.Count - 1], table[table.Count - 2]) * ((dynamic)value - (dynamic)table[table.Count - 1].second) + table[table.Count - 1].first;
                    }
                    else return table[table.Count - 1].first;
                }
            }
            else if (table.Count == 1) return table[0].first;
            else return (X)Convert.ChangeType(value, typeof(X));
        }//線形補間(逆関数)
#endif //NET40_OR_GREATER || NETFX_CORE
    }
    struct pair<X, Y>
	{
		public X first { get; set; }
		public Y second { get; set; }

		public pair(X x, Y y)
		{
			first = x; second = y;
		}
    }

#if !(NET40_OR_GREATER || NETFX_CORE)
    internal class CURRENT_SET : IEquatable<CURRENT_SET>, IComparable<CURRENT_SET>
	{
		public float first;
		public float second;
		public CURRENT_SET(float _first, float _second)
		{
			first = _first; second = _second;
		}
		public bool Equals(CURRENT_SET _right)
		{
			return this.first.Equals(_right.first) && this.second.Equals(_right.second);
		}
		public override bool Equals(object o)
		{
			return true;
		}
		public override int GetHashCode()
		{
			return 0;
		}
		static public bool operator ==(CURRENT_SET _left, CURRENT_SET _right)
		{
			return _left.first == _right.first && _left.second == _right.second;
		}
		static public bool operator !=(CURRENT_SET _left, CURRENT_SET _right)
		{
			return _left.first != _right.first || _left.second != _right.second;
		}

		public int CompareTo(CURRENT_SET _right)
		{
			return this.first.CompareTo(_right.first);
		}

		static public bool operator >(CURRENT_SET _left, CURRENT_SET _right)
		{
			return _left.first > _right.first;
		}
		static public bool operator <(CURRENT_SET _left, CURRENT_SET _right)
		{
			return _left.first < _right.first;
		}

		//csv読み込み
		public static void makeTableFromCsv(string name,//csvファイルの絶対パス
			 List<CURRENT_SET> table,//テーブルとなるList
			int row = 1,//csvファイルで読み込む列
			string enc = ".UTF-8"
		)
		{
			if (File.Exists(name))
			{
					table.Clear();
#if VISUAL_BASIC
                using (TextFieldParser csvReader = new TextFieldParser(name, Encoding.GetEncoding(enc)))
                {
                    csvReader.CommentTokens = new string[] { "#" };
                    csvReader.SetDelimiters(new string[] { "," });
                    csvReader.HasFieldsEnclosedInQuotes = true;

                    //1行目はヘッダーなので飛ばす。
                    csvReader.ReadLine();
                    while (!csvReader.EndOfData)
                    {
                        string[] columun = csvReader.ReadFields();
                        if (!string.IsNullOrEmpty(columun[row]))
                        {
                            table.Add(new CURRENT_SET(float.Parse(columun[0]), float.Parse(columun[row])));
                        }
                    }
                    csvReader.Close();
                }
#else
                    using (System.IO.StreamReader csv = new System.IO.StreamReader(System.IO.File.OpenRead(name), System.Text.Encoding.GetEncoding(enc)))
                    {
                        string loadline;
                        loadline = csv.ReadLine();
                        while (!csv.EndOfStream)
                        {
                            string[] columun;
                            loadline = csv.ReadLine();
                            loadline = LoadBveText.cleanUpBveStr(loadline);
                            if (loadline.Length > 0)
                            {
                                columun = loadline.Split(',');
 //                               foreach (var item in columun)
 //                               {
 //                                   LoadBveText.eraseSpace(item);
 //                               }
                                if (!string.IsNullOrEmpty(columun[row]))
                                {
                                    table.Add(new CURRENT_SET(float.Parse(columun[0]), float.Parse(columun[row])));
                                }
                            }
                        }
                    }
#endif
                    if (table.Count > 0)
				{
					table.Sort();
				}
				table.TrimExcess();
			}
		}

		//csv読み込み(インデックスリスト使用)
		public static void makeTableFromCsv(
		string name,//csvファイルの絶対パス
		List<List<CURRENT_SET>> table,//テーブルとなるvector
		List<int> index_list,//csvファイルで読み込む列のリスト
		string enc = ".UTF-8"
	)
		{
			table.Clear();
			foreach (var a in index_list) table.Add(new List<CURRENT_SET>());
			if (File.Exists(name))
            {
#if VISUAL_BASIC
				using (TextFieldParser csvReader = new TextFieldParser(name, Encoding.GetEncoding(enc)))
                {
                    csvReader.CommentTokens = new string[] { "#" };
                    csvReader.SetDelimiters(new string[] { "," });
                    csvReader.HasFieldsEnclosedInQuotes = true;

                    //1行目はヘッダーなので飛ばす。
                    csvReader.ReadLine();
                    while (!csvReader.EndOfData)
                    {
                        string[] columun = csvReader.ReadFields();
                        int size = columun.Count();
                        var itr = table.GetEnumerator();
                        foreach (var a in index_list)
                        {
                            if (size > a && itr.MoveNext())
                            {
                                if (!String.IsNullOrEmpty(columun[a]))
                                {
                                    itr.Current.Add(new CURRENT_SET(float.Parse(columun[0]), float.Parse(columun[a])));
                                }
                            }
                        }
                    }
                    csvReader.Close();
                }
#else
                using (System.IO.StreamReader csv = new System.IO.StreamReader(System.IO.File.OpenRead(name), System.Text.Encoding.GetEncoding(enc)))
                {
                    string loadline;
                    loadline = csv.ReadLine();
                    while (!csv.EndOfStream)
                    {
                        string[] columun;
                        loadline = csv.ReadLine();
                        loadline = LoadBveText.cleanUpBveStr(loadline);
                        if (loadline.Length > 0)
                        {
                            columun = loadline.Split(',');
//                            foreach (var item in columun)
//                            {
//                                LoadBveText.eraseSpace(item);
//                            }
                            int size = columun.Count();
                            var itr = table.GetEnumerator();
                            foreach (var a in index_list)
                            {
                                if (size > a && itr.MoveNext())
                                {
                                    if (!String.IsNullOrEmpty(columun[a]))
                                    {
                                        itr.Current.Add(new CURRENT_SET(float.Parse(columun[0]), float.Parse(columun[a])));
                                    }
                                }
                            }
                        }
                    }
                }
#endif
            foreach (var a in table)
				{
					if (a.Count > 0)
					{
						a.Sort();
					}
					a.TrimExcess();
				}

				table.TrimExcess();
			}
		}

		//2点間を通る直線の傾き
		public static float slope(CURRENT_SET p1, CURRENT_SET p2)
		{
			return (p2.second - p1.second) / (p2.first - p1.first);
		}

		//2点間を通る直線の傾き(逆関数)
		public static float slopeInv(CURRENT_SET p1, CURRENT_SET p2)
		{
			return (p2.first - p1.first) / (p2.second - p1.second);
		}

		//2点間を通る直線の傾き
		public static float slope(float x1, float y1, float x2, float y2)
		{
			return (y2 - y1) / (x2 - x1);
		}

		//2点間を通る直線のy切片
		public static float intercept(CURRENT_SET p1, CURRENT_SET p2)
		{
			return -slope(p1, p2) * p1.first + p1.second;
		}

		//2点間を通る直線のy切片
		public static float intercept(float x1, float y1, float x2, float y2)
		{
			return -slope(x1, y1, x2, y2) * x1 + y1;
		}

		//2点間を通る直線の方程式(first:傾き，second:y切片)
		public static CURRENT_SET linear(CURRENT_SET p1, CURRENT_SET p2)
		{
			return new CURRENT_SET(slope(p1, p2), intercept(p1, p2));
		}

		//2点間を通る直線の方程式(first:傾き，second:y切片)
		public static CURRENT_SET linear(float x1, float y1, float x2, float y2)
		{
			return new CURRENT_SET(slope(x1, y1, x2, y2), intercept(x1, y1, x2, y2));
		}
		//線形補間
		public static float interpolation(
		   float value,//x座標（速度）
		   List<CURRENT_SET> table//テーブルとなるvector
	   )
		{
			if (table.Count > 1)
			{
				int j = 0;
				foreach (var a in table)
				{
					if (a.first >= value) break;
					j++;
				}
				if (j > 0 && j < table.Count)
				{
					if (table[j].first != table[j - 1].first)
					{
						return slope(table[j], table[j - 1]) * (value - table[j].first) + table[j].second;
					}
					else return table[j].second;
				}
				else if (j == 0)
				{
					if (table[j + 1].first != table[j].first)
					{
						return slope(table[j + 1], table[j]) * (value - table[j + 1].first) + table[j + 1].second;
					}
					else return table[j].second;
				}
				else
				{
					if (table[table.Count - 1].first != table[table.Count - 2].first)
					{
						return slope(table[table.Count - 1], table[table.Count - 2]) * (value - table[table.Count - 1].first) + table[table.Count - 1].second;
					}
					else return table[table.Count - 1].second;
				}
			}
			else if (table.Count == 1) return table[0].second;
			else return value;
		}//線形補間

		//線形補間(逆関数)
		public static float interpolationInv(
		float value,//Y座標（距離？）
		List<CURRENT_SET> table//テーブルとなるvector
		)
		{
			if (table.Count > 1)
			{
				int j = 0;
				foreach (var a in table)
				{
					if (a.second >= value) break;
					j++;
				}
				if (j > 0 && j < table.Count)
				{
					if (table[j].second != table[j - 1].second)
					{
						return slopeInv(table[j], table[j - 1]) * (value - table[j].second) + table[j].first;
					}
					else return table[j].first;
				}
				else if (j == 0)
				{
					if (table[j + 1].second != table[j].second)
					{
						return slopeInv(table[j + 1], table[j]) * (value - table[j + 1].second) + table[j + 1].first;
					}
					else return table[j].first;
				}
				else
				{
					if (table[table.Count - 1].second != table[table.Count - 2].second)
					{
						return slopeInv(table[table.Count - 1], table[table.Count - 2]) * (value - table[table.Count - 1].second) + table[table.Count - 1].first;
					}
					else return table[table.Count - 1].first;
				}
			}
			else if (table.Count == 1) return table[0].first;
			else return value;
		}//線形補間(逆関数)



	}
#endif//!NET40_OR_GREATER
}
