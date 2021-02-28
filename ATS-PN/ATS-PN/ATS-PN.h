#ifndef _ATS_PN_INCLUDED_
#define _ATS_PN_INCLUDED_


#ifndef _WIN32_WINNT
#include <winsdkver.h>
#define _WIN32_WINNT _WIN32_WINNT_WIN10
//#define _WIN32_WINNT _WIN32_WINNT_WINBLUE
//#define _WIN32_WINNT _WIN32_WINNT_WIN8
//#define _WIN32_WINNT _WIN32_WINNT_WIN7
#endif //_WIN32_WINNT
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
#endif
#define NOMINMAX
#include <Windows.h>
#include <mfapi.h>
#include <fstream>
#include <filesystem>
#include <memory>
#include <array>
#include <vector>

#include "atsplugin.h"
#include "CATSPN.h"
#include "CDoorcontrol.h"
#include "CAutoAnnounce.h"
#include "CTraponBackGround.h"

namespace PN_Beacon
{
	inline constexpr int SpdLim = 6;//PN制御速度制限
	inline constexpr int StopPatern = 12;//PN制御駅通防止
	inline constexpr int Defeat = 16;//PN制御速度制限解除
	inline constexpr int Terminal = 91;//終端防護
	inline constexpr int Timetable = 100;//時刻表の変更
	inline constexpr int Halt = 105;//駅通防止チャイム
	inline constexpr int LineLim = 130;//線区最高速度
};

inline HMODULE g_hModule;
inline int g_svcBrake;//常用最大
inline int g_emgBrake;//非常ブレーキ
inline int g_Power;//力行ノッチ
inline int g_Brake;//ブレーキノッチ
inline int g_Reverser;//レバーサ
inline float g_TrainSpeed;//速度
inline int g_time;//時刻
inline int g_deltaT;//前のフレームとの時刻の差
inline double g_location;
inline double g_deltaL;//1フレームで進んだ距離
inline int g_tmr;//停車駅名点滅タイマー
inline int g_StopSta;//停車駅名
inline int g_AstTimer;//アスタリスク点滅タイマー
inline bool g_Aster;//アスタリスク
inline int g_timetable = 0;//時刻表
inline bool g_delete_push = false;//Deleteが押されている間True
inline bool g_ICcard_insert = false;//ICカード挿入状態

inline CTraponBackGround g_TraponBackGround;//トラポン背景の出力

inline winrt::com_ptr<IXAudio2> pXAudio2;
inline IXAudio2MasteringVoice* pMasteringVoice = nullptr;
inline std::filesystem::path g_module_dir;
inline CSourceVoice g_Ding;
inline bool g_Space = false;
inline CSourceVoice g_Ding1;
inline CSourceVoice g_Ding2;
inline CATSPN g_pncontrol;
inline std::unique_ptr<CDoorcontrol> g_door{ nullptr };
inline std::unique_ptr<CAutoAnnounce> g_announce{ nullptr };
inline std::array<int, 2> g_sta_no = { 0,0 };
inline bool g_ShasyouBell = false;
inline bool g_Bell1 = false;
inline bool g_Bell2 = false;
inline int g_belltimer = 0;


#endif // !_ATS_PN_INCLUDED_