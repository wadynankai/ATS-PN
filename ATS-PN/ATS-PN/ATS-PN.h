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
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <chrono>
#include "atsplugin.h"
#include "..\..\common\LoadBveText.h"
#include "CATSPN.h"
#include "CDoorcontrol.h"
#include "CAutoAnnounce.h"
#include "CTraponBackGround.h"
#include "..\..\common\CAtsSound.h"
#include "..\..\common\CSourceVoice.h"

#ifdef EXCEPTION
#include <source_location>
#endif // EXCEPTION

using namespace std::literals::chrono_literals;


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
inline int g_svcBrake;//常用最大
inline int g_emgBrake;//非常ブレーキ
inline int g_Power;//力行ノッチ
inline int g_Brake;//ブレーキノッチ
inline int g_Reverser;//レバーサ
inline float g_TrainSpeed;//速度
inline std::chrono::time_point<std::chrono::milliseconds> g_time;//時刻
inline std::chrono::milliseconds g_deltaT;//前のフレームとの時刻の差
inline double g_location;
inline double g_deltaL;//1フレームで進んだ距離
inline std::chrono::milliseconds g_tmr;//停車駅名点滅タイマー
inline int g_StopSta;//停車駅名
inline std::chrono::milliseconds g_AstTimer;//アスタリスク点滅タイマー
inline bool g_Aster;//アスタリスク
inline int g_timetable = 0;//時刻表
inline bool g_home_push = false;//Homeが押されている間True
inline bool g_insert_push = false;//Insertが押されている間True
inline bool g_delete_push = false;//Deleteが押されている間True
inline bool g_PgUp_push = false;//Deleteが押されている間True
inline bool g_PgDn_push = false;//Deleteが押されている間True

inline winrt::com_ptr<IXAudio2> pXAudio2;
inline IXAudio2MasteringVoice* pMasteringVoice = nullptr;
inline std::filesystem::path g_module_dir;
inline constexpr winrt::Windows::Foundation::TimeSpan dingDuration = 42ms;
inline bool g_Space = false;

inline CSourceVoice g_Ding = nullptr, g_Ding1 = nullptr, g_Ding2 = nullptr;
//inline std::array<int, 2> g_sta_no = { 0,0 };
inline bool g_ShasyouBell = false;//車掌ベル機能有効
inline bool g_Bell1 = false;
inline bool g_Bell2 = false;
inline std::chrono::milliseconds g_belltimer = 0ms;

inline CAtsSound<26> g_trapon_push;
inline CAtsSound<27> g_trapon_release;
inline CAtsSound<28> g_trapon_on;
inline CAtsSound<29> g_trapon_off;


#endif // !_ATS_PN_INCLUDED_