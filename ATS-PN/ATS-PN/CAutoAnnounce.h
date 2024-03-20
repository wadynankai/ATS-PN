#ifndef _CAUTO_ANNOUNCE_INCLUDED_
#define _CAUTO_ANNOUNCE_INCLUDED_
#define NOMINMAX

#include <limits>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include "..\..\common\LoadBveText.h"
#include "..\..\common\CSourceVoice.h"

#ifdef EXCEPTION
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
#endif
#include <windows.h>
#include <source_location>
#endif // EXCEPTION


using namespace std::literals::chrono_literals;

namespace dep_distance
{
	constexpr double commuter = 50.0;//通勤者出発後のアナウンスは50m走行後に鳴らす。
	constexpr double rapi_t_first = 80.0;//ラピート始発駅のアナウンスは80m走行後に鳴らす。
	constexpr double southern_first = 180.0;//サザンプレミアム始発駅のアナウンスは150m走行後に鳴らす。
	constexpr double honsen_ltd_exp = 300.0;//南海線特急出発後のアナウンスは300m走行後に鳴らす。
	constexpr double koya_ltd_exp = 100.0;//高野線特急始発駅のアナウンスは100m走行後に鳴らす。
};

enum class AnnounceMode
{
	manual, Commuter, Rapit, Southern, Koya
};
struct AnnounceSet//CSVファイルの行を保存
{
	int sta_no = 0;//駅番号
	std::filesystem::path name1;//出発後のアナウンス
	AnnounceMode mode = AnnounceMode::Commuter;
	double location1 = std::numeric_limits<double>::max();//到着前のアナウンスの位置
	std::filesystem::path name2;//到着前のアナウンス
	double location2 = std::numeric_limits<double>::max();//到着前のアナウンスの位置
};
class CAutoAnnounce
{
public:
	~CAutoAnnounce()noexcept;
	static void CreateInstance(const std::filesystem::path& moduleDir, const winrt::com_ptr<IXAudio2>& pXau2, std::chrono::milliseconds& DelT)
	{
		if (!instance)instance.reset(new CAutoAnnounce(moduleDir, pXau2, DelT));
	}
/*	static void Delete()noexcept
	{
		instance.reset();
	}*/
	static std::unique_ptr<CAutoAnnounce>& GetInstance()noexcept
	{
		return instance;
	}
	void setTrainNo(int number);
	inline void Running(const double& loc);
	inline void Halt(const int no) noexcept;
	inline void DoorCls(void) noexcept;
	float micGauge = 0.0f;

private:
	CAutoAnnounce() = delete;
	CAutoAnnounce(const std::filesystem::path& moduleDir, const winrt::com_ptr<IXAudio2>& pXau2, std::chrono::milliseconds& DelT);
	CAutoAnnounce& operator=(CAutoAnnounce&) = delete;
	CAutoAnnounce& operator=(CAutoAnnounce&&) = delete;
	inline static std::unique_ptr<CAutoAnnounce> instance;
	std::filesystem::path m_module_dir;//プラグインのディレクトリ
	std::filesystem::path m_table_dir;//設定ファイルのディレクトリ
	int m_trainNo = 0;//時刻表番号
	int m_staNo = 0;//駅番号
	std::chrono::milliseconds& m_DelT;//1フレームの時間
	double m_Location = 0.0, m_Location_pre = 0.0;//距離程
//	double m_LocationOrigin = 0.0;//扉が閉まった瞬間の距離程
//	double m_RunDistance = 0.0, m_RunDistance_pre = 0.0;//出発してからの距離
	bool m_set_no = false;//一度でも105番地上子を踏んだことがあったらtrue
	bool m_pilotLamp = false;//ドアが閉まっていたらtrue
	AnnounceSet m_first{};//始発駅の設定を保存
	std::vector<AnnounceSet> m_A_Set;//始発駅以外の設定を保存
	AnnounceMode m_AnnounceMode{ AnnounceMode::Commuter };
	double m_A_Loc1{ std::numeric_limits<double>::max() };//出発放送の距離程を保存
	double m_A_Loc2{ std::numeric_limits<double>::max() };//到着放送の距離程を保存

	winrt::com_ptr<IXAudio2> m_pXAudio2;
	CSourceVoice m_Announce1{}, m_Announce2{};
	std::filesystem::path* m_pAnnounce1 = nullptr, * m_pAnnounce2 = nullptr;
	std::thread m_thread1{}, m_thread2{};
	bool m_first_time = true;

//	std::wofstream ofs;
};



inline void CAutoAnnounce::Running(const double& loc)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	static int time_pre = 0;//前フレームの時刻
	m_Location = loc;//現在の位置
//	m_RunDistance = m_Location - m_LocationOrigin;//出発してからの距離
	if (!m_first_time)
	{
		if (m_Announce1.flag && m_pAnnounce1 && !m_thread2.joinable())//Annouce1と2を同時に読み込んでしまうとメディアファンデーションの影響でフリーズする。
		{
			if (m_thread1.joinable())
			{
				m_thread1.join();
			}
//			Announce1Load = true;//速い方が安全なのでスレッド代入前に行う。
			m_thread1 = std::thread([&] {
				m_Announce1.reset(m_pXAudio2, *m_pAnnounce1, 0, XAUDIO2_VOICE_NOPITCH);
//				Announce1Load = false;
				});//放送を登録
			m_Announce1.flag = false;
		}
		if (m_Announce2.flag && m_pAnnounce2 && !m_thread1.joinable())//Annouce1と2を同時に読み込んでしまうとメディアファンデーションの影響でフリーズする。
		{
			if (m_thread2.joinable())
			{
				m_thread2.join();
			}
//			Announce2Load = true;//速い方が安全なのでスレッド代入前に行う。
			m_thread2 = std::thread([&] {
				m_Announce2.reset(m_pXAudio2, *m_pAnnounce2, 0, XAUDIO2_VOICE_NOPITCH);
//				Announce2Load = false;
				});//放送を登録
			m_Announce2.flag = false;
		}

		if (m_DelT >= 1000ms || m_DelT <= 0ms)//駅に移動したとき
		{
			if (m_thread1.joinable())
			{
				m_thread1.join();
			}
			if (m_thread2.joinable())
			{
				m_thread2.join();
			}
			m_Announce1->Stop();
			m_Announce2->Stop();
		}
		else
		{
			if (m_Location_pre < m_A_Loc1 && m_Location >= m_A_Loc1)
			{
				if (m_thread1.joinable())
				{
					m_thread1.join();
				}
				if (m_Announce2.isRunning())m_Announce2->Stop();
				m_Announce1->Start();
			}
			if (m_Location_pre < m_A_Loc2 && m_Location >= m_A_Loc2)
			{
				if (m_thread2.joinable())
				{
					m_thread2.join();
				}
				if (m_Announce1.isRunning())m_Announce1->Stop();
				m_Announce2->Start();
			}
		}
	}
	else
	{
		m_first_time = false;
	}

	//micGauge = std::min(m_Announce1.getLevel() + m_Announce2.getLevel(), 1.0f);

	m_Location_pre = m_Location;
	//	m_RunDistance_pre = m_RunDistance;
#ifdef EXCEPTION
	}
	catch (std::exception& ex)
	{
		MessageBoxA(nullptr, ex.what(), std::source_location::current().function_name(), MB_OK);
	}
	catch (winrt::hresult_error& hr)
	{
		std::wstringstream ss1, ss2;
		ss1 << L"エラーコード：" << std::hex << hr.code() << L"\n" << hr.message().c_str();
		ss2 << std::source_location::current().function_name();
		MessageBox(nullptr, ss1.str().c_str(), ss2.str().c_str(), MB_OK);
	}
#endif // EXCEPTION
}


inline void CAutoAnnounce::Halt(const int no) noexcept
{
	if (!m_set_no)m_set_no = true;
	m_staNo = no;
}

inline void CAutoAnnounce::DoorCls(void) noexcept
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
		//	m_LocationOrigin = m_Location;//駅発車時の位置を登録（出発後の放送に使用）
		if (m_set_no)//始発駅以外
		{
			for (const auto& a : m_A_Set)
			{
				if (a.sta_no == m_staNo)
				{
					if (!a.name1.empty())
					{
						m_Announce1.flag = true;
						m_pAnnounce1 = const_cast<std::filesystem::path*>(&a.name1);//次駅放送ファイル名を登録
					}
					else
					{
						m_Announce1 = nullptr;//前の放送を消去
					}
					if (!a.name2.empty())
					{
						m_Announce2.flag = true;
						m_pAnnounce2 = const_cast<std::filesystem::path*>(&a.name2);//到着放送ファイル名を登録
					}
					else
					{
						m_Announce2 = nullptr;//前の放送を消去
					}
					switch (a.mode)//出発後放送の位置を登録
					{
					case AnnounceMode::Commuter:
						m_A_Loc1 = m_Location + dep_distance::commuter;
						break;
					case AnnounceMode::Rapit:
					case AnnounceMode::Southern:
						m_A_Loc1 = m_Location + dep_distance::honsen_ltd_exp;
						break;
					case AnnounceMode::Koya:
						m_A_Loc1 = m_Location + dep_distance::koya_ltd_exp;
						break;
					case AnnounceMode::manual:
						m_A_Loc1 = a.location1;
						break;
					}
					m_A_Loc2 = a.location2;//到着前放送の位置を登録
					break;
				}
			}
		}
		else//始発駅
		{
			if (!m_first.name1.empty())
			{
				m_Announce1.flag = true;
				m_pAnnounce1 = &m_first.name1;//次駅放送ファイル名を登録
			}
			if (!m_first.name2.empty())
			{
				m_Announce2.flag = true;
				m_pAnnounce2 = &m_first.name2;//到着放送ファイル名を登録
			}
			switch (m_first.mode)//出発後放送の位置を登録
			{
			case AnnounceMode::Commuter:
				m_A_Loc1 = m_Location + dep_distance::commuter;
				break;
			case AnnounceMode::Rapit:
				m_A_Loc1 = m_Location + dep_distance::rapi_t_first;
				break;
			case AnnounceMode::Southern:
				m_A_Loc1 = m_Location + dep_distance::southern_first;
				break;
			case AnnounceMode::Koya:
				m_A_Loc1 = m_Location + dep_distance::koya_ltd_exp;
				break;
			case AnnounceMode::manual:
				m_A_Loc1 = m_first.location1;
				break;
			}
			m_A_Loc2 = m_first.location2;//到着前放送の位置を登録
		}
#ifdef EXCEPTION
	}
	catch (std::exception& ex)
	{
		MessageBoxA(nullptr, ex.what(), std::source_location::current().function_name(), MB_OK);
	}
	catch (winrt::hresult_error& hr)
	{
		std::wstringstream ss1, ss2;
		ss1 << L"エラーコード：" << std::hex << hr.code() << L"\n" << hr.message().c_str();
		ss2 << std::source_location::current().function_name();
		MessageBox(nullptr, ss1.str().c_str(), ss2.str().c_str(), MB_OK);
	}
#endif // EXCEPTION
};


#endif // !_CAUTO_ANNOUNCE_INCLUDED_
