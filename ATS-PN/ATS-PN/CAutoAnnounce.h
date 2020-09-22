#pragma once
//#include "pch.h"
#include <vector>
#include <string>
#include "LoadBveText.h"

constexpr auto DEP_DISTANCE_COM = 50.0;//通勤者出発後のアナウンスは50m走行後に鳴らす。
constexpr auto DEP_DISTANCE_R_FIRST = 80.0;//ラピート始発駅のアナウンスは80m走行後に鳴らす。
constexpr auto DEP_DISTANCE_S_FIRST = 180.0;//サザンプレミアム始発駅のアナウンスは150m走行後に鳴らす。
constexpr auto DEP_DISTANCE_LIM = 300.0;//南海線特急出発後のアナウンスは300m走行後に鳴らす。
constexpr auto DEP_DISTANCE_K = 100.0;//高野線特急始発駅のアナウンスは100m走行後に鳴らす。
enum class AnnounceMode
{
	manual, Commuter, Rapit, Southern, Koya
};
struct AnnounceSet//CSVファイルの行を保存
{
	int sta_no = 0;//駅番号
	std::wstring name1;//出発後のアナウンス
	AnnounceMode mode = AnnounceMode::Commuter;
	double location1 = DBL_MAX;//到着前のアナウンスの位置
	std::wstring name2;//到着前のアナウンス
	double location2 = DBL_MAX;//到着前のアナウンスの位置
};
class CAutoAnnounce
{
public:
	CAutoAnnounce(const std::wstring& moduleDir, IXAudio2* pXau2 = nullptr);
	~CAutoAnnounce();
	void setTrainNo(int);
	void Running(const double,const int);
	void Halt(const int);
	void DoorCls(void);

private:
	std::wstring m_module_dir;//プラグインのディレクトリ
	std::wstring m_table_dir;//設定ファイルのディレクトリ
	int m_trainNo = 0;//時刻表番号
	int m_staNo = 0;//駅番号
	double m_Location = 0.0, m_Location_pre = 0.0;//距離程
//	double m_LocationOrigin = 0.0;//扉が閉まった瞬間の距離程
//	double m_RunDistance = 0.0, m_RunDistance_pre = 0.0;//出発してからの距離
	bool m_set_no = false;//一度でも105番地上子を踏んだことがあったらtrue
	AnnounceSet m_first;//始発駅の設定を保存
	std::vector<AnnounceSet> m_A_Set;//始発駅以外の設定を保存
	AnnounceMode m_AnnounceMode;
	double m_A_Loc1 = DBL_MAX;//出発放送の距離程を保存
	double m_A_Loc2 = DBL_MAX;//到着放送の距離程を保存

	IXAudio2* m_pXAudio2 = nullptr;
	CSourceVoice* m_Announce1 = nullptr, * m_Announce2 = nullptr;


//	std::wofstream ofs;
};

