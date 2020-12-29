#ifndef _CAUTO_ANNOUNCE_INCLUDED_
#define _CAUTO_ANNOUNCE_INCLUDED_
#define NOMINMAX
#include <fstream>
#include <locale>
#include <vector>
#include <filesystem>
#include <algorithm>
#include "..\..\common\LoadBveText.h"
#include "..\..\common\CSourceVoice.h"

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
	CAutoAnnounce(const std::filesystem::path& moduleDir, IXAudio2* pXau2 = nullptr);
	~CAutoAnnounce();
	void setTrainNo(int);
	inline void Running(const double,const int);
	inline void Halt(const int);
	inline void DoorCls(void);
	float micGauge;

private:
	std::filesystem::path m_module_dir;//プラグインのディレクトリ
	std::filesystem::path m_table_dir;//設定ファイルのディレクトリ
	int m_trainNo = 0;//時刻表番号
	int m_staNo = 0;//駅番号
	double m_Location = 0.0, m_Location_pre = 0.0;//距離程
//	double m_LocationOrigin = 0.0;//扉が閉まった瞬間の距離程
//	double m_RunDistance = 0.0, m_RunDistance_pre = 0.0;//出発してからの距離
	bool m_set_no = false;//一度でも105番地上子を踏んだことがあったらtrue
	bool m_pilotLamp = false;//ドアが閉まっていたらtrue
	AnnounceSet m_first;//始発駅の設定を保存
	std::vector<AnnounceSet> m_A_Set;//始発駅以外の設定を保存
	AnnounceMode m_AnnounceMode;
	double m_A_Loc1 = std::numeric_limits<double>::max();//出発放送の距離程を保存
	double m_A_Loc2 = std::numeric_limits<double>::max();//到着放送の距離程を保存

	IXAudio2* m_pXAudio2 = nullptr;
	CSourceVoice m_Announce1, m_Announce2;


//	std::wofstream ofs;
};



inline void CAutoAnnounce::Running(const double loc, const int time)
{
	static int time_pre = 0;//前フレームの時刻
	int delT = 0;//1フレームの時間
	delT = time - time_pre;
	m_Location = loc;//現在の位置
//	m_RunDistance = m_Location - m_LocationOrigin;//出発してからの距離

	if (m_Location_pre < m_A_Loc1 && m_Location >= m_A_Loc1)m_Announce1->Start();
	if (m_Location_pre < m_A_Loc2 && m_Location >= m_A_Loc2)m_Announce2->Start();
	if (delT >= 1000 || delT < 0)//駅に移動したとき
	{
		if (m_Announce1)m_Announce1->Stop();
		if (m_Announce2)m_Announce2->Stop();
	}

	micGauge = std::min(m_Announce1.getLevel() + m_Announce2.getLevel(), 1.0f);

	m_Location_pre = m_Location;
	//	m_RunDistance_pre = m_RunDistance;
	time_pre = time;
}


inline void CAutoAnnounce::Halt(const int no)
{
	if (!m_set_no)m_set_no = true;
	m_staNo = no;
}

inline void CAutoAnnounce::DoorCls(void)
{
	//	m_LocationOrigin = m_Location;//駅発車時の位置を登録（出発後の放送に使用）
	if (m_set_no)//始発駅以外
	{
		for (const auto& a : m_A_Set)
		{
			if (a.sta_no == m_staNo)
			{
				HRESULT hr;
				bool mfStarted = false;//メディアファンデーションプラットフォームを初期化したらTRUEにする。
				hr = MFStartup(MF_VERSION);// メディアファンデーションプラットフォームの初期化
				mfStarted = SUCCEEDED(hr);//初期化出来たらTRUEにする。
				if (!a.name1.empty())
				{
					m_Announce1.reset(m_pXAudio2, a.name1, 0, XAUDIO2_VOICE_NOPITCH);//放送を登録
				}
				else m_Announce1.reset(nullptr);//前の放送を消去
				if (!a.name2.empty())
				{
					m_Announce2.reset(m_pXAudio2, a.name2, 0, XAUDIO2_VOICE_NOPITCH);//放送を登録
				}
				else m_Announce2.reset();//前の放送を消去
				if (mfStarted)MFShutdown();// メディアファンデーションプラットフォームが初期化されていたら終了
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
				m_Announce1->SetVolume(1.0f);//音量を1に設定する（これがないと，音量0になる）
				m_Announce2->SetVolume(1.0f);//音量を1に設定する（これがないと，音量0になる）
				break;
			}
		}
	}
	else//始発駅
	{
		HRESULT hr;
		bool mfStarted = false;//メディアファンデーションプラットフォームを初期化したらTRUEにする。
		hr = MFStartup(MF_VERSION);// メディアファンデーションプラットフォームの初期化
		mfStarted = SUCCEEDED(hr);//初期化出来たらTRUEにする。
		if (!m_first.name1.empty())m_Announce1.reset(m_pXAudio2, m_first.name1, 0, XAUDIO2_VOICE_NOPITCH);//放送を登録
		if (!m_first.name2.empty())m_Announce2.reset(m_pXAudio2, m_first.name2, 0, XAUDIO2_VOICE_NOPITCH);//放送を登録
		if (mfStarted)MFShutdown();// メディアファンデーションプラットフォームが初期化されていたら終了
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
		m_Announce1->SetVolume(1.0f);//音量を1に設定する（これがないと，音量0になる）
		m_Announce2->SetVolume(1.0f);//音量を1に設定する（これがないと，音量0になる）
	}
}


#endif // !_CAUTO_ANNOUNCE_INCLUDED_
