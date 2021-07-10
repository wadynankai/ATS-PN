#ifndef _CATSPN_INCLUDED_
#define _CATSPN_INCLUDED_

#include <limits>
#include <vector>
#include <filesystem>
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
#include "atsplugin.h"
#include "..\..\common\CURRENT_SET.h"
#include "..\..\common\CAtsSound.h"
#include "..\..\common\CAtsSoundLoop.h"

class CATSPN
{
public:
	static void CreateInstance(std::filesystem::path& module_dir, float& Speed, int& DelT, double& DelL, int& Bpos)
	{
		if (!pInstance)pInstance.reset(new CATSPN(module_dir, Speed, DelT, DelL, Bpos));
	}
/*	static void Delete()noexcept
	{
		pInstance.reset();
	}*/
	static std::unique_ptr<CATSPN>& GetInstance()noexcept
	{
		return pInstance;
	}
	// 初期化
	void initATSPN(void)noexcept;
	//リセットボタン
	void resetATSPN(void)noexcept;
	//PN制御実行
	void RunPNcontrol(void)noexcept;
	// 駅通防止
	void haltON(int)noexcept;
	void stopPattern(int)noexcept;
	void halt(void)noexcept;
	void haltOFF(void)noexcept;
	// 線区最高速度
	void LineMax(int)noexcept;
	// 速度制限区間
	void LimitSpeedON(int)noexcept;
	void LimitSpeed(void)noexcept;
	void LimitSpeedOFF(void)noexcept;
	// 終端防護（非常指令）
	void TerminalSafetyON(int)noexcept;
	void TerminalSafety(void)noexcept;
	//非常ブレーキ指令
	bool emgBrake = false;
	//常用最大ブレーキ指令
	bool svcBrake = false;
	//PN制御表示
	bool PNcontrolDisp = false;
	//速度制限表示
	int SpeedLimitDisp = 0;
	//駅通防止表示
	int haltDisp = 0;
	//終端防護表示
	int TerminalSafetyDisp = 0;
	//駅名表示
	int StationName = 0;
	//P接近
	bool PatternApproachDisp = false;
	//駅通防止チャイム
	CAtsSound<6> HaltSound;
	//パターン接近音声
	CAtsSoundLoop<11> ApproachSound;

private:
	//パターン読み込み
	CATSPN() = delete;
	CATSPN(std::filesystem::path& module_dir, float& Speed, int& DelT, double& DelL, int& Bpos);
	CATSPN(CATSPN&) = delete;
	CATSPN(CATSPN&&) = delete;
	CATSPN& operator=(CATSPN&) = delete;
	CATSPN& operator=(CATSPN&&) = delete;
	inline static std::unique_ptr<CATSPN> pInstance;
	//PN制御（駅通防止）
	bool m_halt = false;
	//PN制御（速度制限）
	bool m_LimitSpeed = false;
	//PN制御（終端防護)
	bool m_TerminalSafety = false;
	//P接近（駅通防止）
	bool m_halt_App = false;
	//P接近（速度制限）
	bool m_LimitSpeed_App = false;
	//P接近（終端防護）
	bool m_TerminalSafety_App = false;
	//駅番号
	int m_Sta_No = 0;
	//駅番号点滅カウンタ
	int m_Sta_count = 0;
	//駅番号点滅タイマー
	int m_Sta_tmr = 0;
	//駅通防止停止距離
	double m_halt_dist = 0.0;
	//速度制限速度
	float m_LimitSpeed_Speed = std::numeric_limits<float>::max();
	//速度制限開始距離
	double m_LimitSpeed_dist = 0.0;
	//終端防護距離
	double m_Terminal_Dist = 0.0;
	//線区最高速度
	double m_Line_Max_Speed = std::numeric_limits<float>::max();
	//駅通防止パターンあり
	bool m_halt_P = false;
	//線区最高速度ブレーキ
	bool m_LineMaxSpeed_b = false;
	//駅通防止ブレーキ
	bool m_halt_b = false;
	//速度制限ブレーキ
	bool m_LimitSpeed_b = false;
	//終端防護ブレーキ
	bool m_TerminalSafety_b = false;
	//速度
	float& m_TrainSpeed;
	//前回との時刻の差
	int& m_DeltaT;
	//前回との位置の差
	double& m_DeltaL;
	//ブレーキハンドルの位置
	int& m_Brake;
	//ブレーキパターン
	std::vector<DISTANCE_SET> m_pattern;
	//現在の速度からの停止距離
	double m_stopDist = 0.0;
	//制限速度からの停止距離
	double m_stopDistFromLimit = 0.0;
	//ブレーキパターンの最大速度
	float m_pattern_Max = std::numeric_limits<float>::max();
	//ブレーキパターンの誤差
	std::vector<DISTANCE_SET> m_ErrPatten;
	//現在の誤差
	double m_CurrentErr = 0.0;

	static constexpr float m_deceleration = 3.7f * 7.2f;//減速定数（減速度[km/h/s] x 7.2）
	static constexpr float m_approach = m_deceleration * 0.5f;//P接近表示条件
	static constexpr float m_offset = 0.5f;//（車上子オフセット[m]）
};

#endif _CATSPN_INCLUDED_