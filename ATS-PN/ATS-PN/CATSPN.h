#pragma once
#include "atsplugin.h"

#define PN_DECELERATION 26.64f//減速定数（減速度[km/h/s] x 7.2）
#define PN_APPROACH 13.32f
#define PN_OFFSET 0.5f//（車上子オフセット[m]）
class CATSPN
{
public:
	CATSPN();
	~CATSPN();
	// 初期化
	void initATSPN(void);
	//リセットボタン
	void resetATSPN(void);
	//PN制御実行
	void RunPNcontrol(void);
	// 駅通防止
	void haltON(int);
	void stopPattern(int);
	void halt(void);
	void haltOFF(void);
	// 線区最高速度
	void LineMax(int);
	// 速度制限区間
	void LimitSpeedON(int);
	void LimitSpeed(void);
	void LimitSpeedOFF(void);
	// 終端防護（非常指令）
	void TerminalSafetyON(int);
	void TerminalSafety(void);
	//速度
	float* TrainSpeed;
	//前回との時刻の差
	int *DeltaT;
	//非常ブレーキ指令
	bool emgBrake;
	//常用最大ブレーキ指令
	bool svcBrake;
	//PN制御表示
	bool PNcontrolDisp;
	//速度制限表示
	int SpeedLimitDisp;
	//駅通防止表示
	int haltDisp;
	//終端防護表示
	int TerminalSafetyDisp;
	//駅名表示
	int StationName;
	//P接近
	bool PatternApproachDisp;
	//駅通防止チャイム
	int HaltSound;
	//パターン接近音声
	int ApproachSound;

private:
	//PN制御（駅通防止）
	bool m_halt;
	//PN制御（速度制限）
	bool m_LimitSpeed;
	//PN制御（終端防護)
	bool m_TerminalSafety;
	//P接近（駅通防止）
	bool m_halt_App;
	//P接近（速度制限）
	bool m_LimitSpeed_App;
	//P接近（終端防護）
	bool m_TerminalSafety_App;
	//駅番号
	int m_Sta_No;
	//駅番号点滅カウンタ
	int m_Sta_count;
	//駅番号点滅タイマー
	int m_Sta_tmr;
	//駅通防止停止距離
	float m_halt_dist;
	//速度制限速度
	float m_LimitSpeed_Speed;
	//速度制限開始距離
	float m_LimitSpeed_dist;
	//終端防護距離
	float m_Terminal_Dist;
	//線区最高速度
	float m_Line_Max_Speed;
	//駅通防止チャイム（駅に移動対策）
	int m_haltchime;
	//駅通防止チャイム再生済み
	bool m_haltchime_played;
	//駅通防止パターンあり
	bool m_halt_P;
	//線区最高速度ブレーキ
	bool m_LineMaxSpeed_b;
	//駅通防止ブレーキ
	bool m_halt_b;
	//速度制限ブレーキ
	bool m_LimitSpeed_b;
	//終端防護ブレーキ
	bool m_TerminalSafety_b;

};

