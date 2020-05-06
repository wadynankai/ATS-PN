#include "stdafx.h"
#include "CATSPN.h"


CATSPN::CATSPN()
{
}


CATSPN::~CATSPN()
{
}


// 初期化
void CATSPN::initATSPN(void)
{
	m_Line_Max_Speed = 999;
	haltOFF();
	LimitSpeedOFF();
	m_TerminalSafety = false;
	m_TerminalSafety_b = false;
}
// リセットボタン
void CATSPN::resetATSPN(void)
{
	haltOFF();
	LimitSpeedOFF();
	m_TerminalSafety = false;
	m_TerminalSafety_b = false;
}
//PN制御実行
void CATSPN::RunPNcontrol(void)
{
	//パターンの計算
	m_LineMaxSpeed_b = (*TrainSpeed > m_Line_Max_Speed);//線区最高速度を超えたらブレーキ
	halt();
	LimitSpeed();
	TerminalSafety();
	//ブレーキの動作
	svcBrake = (m_halt_b || m_LimitSpeed_b || m_LineMaxSpeed_b);
	emgBrake = m_TerminalSafety_b;

	//表示
	PNcontrolDisp = (m_halt || m_LimitSpeed || m_TerminalSafety);//PN制御
	PatternApproachDisp = (m_halt_App || m_LimitSpeed_App || m_TerminalSafety_App);//P接近
	//P接近音声
	if (PatternApproachDisp)ApproachSound = ATS_SOUND_PLAYLOOPING;
	else ApproachSound = ATS_SOUND_STOP;

	if (m_halt && m_halt_App) haltDisp = 2;//駅通防止赤
	else if (m_halt)haltDisp = 1;//駅通防止白
	else haltDisp = 0;//駅通防止消去

	if (m_LimitSpeed && m_LimitSpeed_App)SpeedLimitDisp = 2;//速度制限赤
	else if (m_LimitSpeed)SpeedLimitDisp = 1;//速度制限白
	else SpeedLimitDisp = 0;//速度制限消去

	if (m_TerminalSafety && m_TerminalSafety_App)TerminalSafetyDisp = 2;//終端防護赤
	else if (m_TerminalSafety)TerminalSafetyDisp = 1;//終端防護白
	else TerminalSafetyDisp = 0;//終端防護消去
}
// 駅通防止
void CATSPN::haltON(int number)
{
	m_halt = true;
	m_Sta_No = number;
	m_haltchime_played = false;
	m_Sta_count = 0;//点滅カウンタリセット
	m_Sta_tmr = 0;//点滅タイマーリセット
	m_haltchime = ATS_SOUND_STOP;
}
void CATSPN::stopPattern(int dist)
{
	if (!m_halt)haltON(0);
	m_halt_P = true;
	m_halt_dist = float(dist);
}
void CATSPN::halt(void)
{
	if (m_halt)
	{
		float pattern = *TrainSpeed * *TrainSpeed / PN_DECELERATION;//停止に必要な距離
		float Approach = *TrainSpeed * *TrainSpeed / PN_APPROACH;
		if (!m_haltchime_played)
		{
			m_haltchime = ATS_SOUND_PLAY;
			m_haltchime_played = true;
		}
		if (m_halt_P)
		{
			float def = *TrainSpeed / 3600.0f * *DeltaT;//1フレームで進んだ距離[m]
			m_halt_dist -= def;
			m_halt_App = (Approach >= m_halt_dist);
			m_halt_b = (pattern >= m_halt_dist && *TrainSpeed > 15);
		}
	}
	else
	{
		m_halt_P = false;
		m_halt_b = false;
		m_halt_App = false;
		m_haltchime = ATS_SOUND_STOP;
	}
	HaltSound = m_haltchime;
	if (m_haltchime == ATS_SOUND_PLAY)m_haltchime = ATS_SOUND_CONTINUE;
	//駅名点滅
	if (m_halt && m_Sta_tmr >= 0 && m_Sta_tmr <= 510 && m_Sta_count < 52)
	{
		if (StationName == 0 || StationName == m_Sta_No * 2 + 2) m_Sta_count += 1;
		StationName = m_Sta_No * 2 + 1;
		m_Sta_tmr += *DeltaT;
	}
	else if (m_halt && m_Sta_tmr > 510 && m_Sta_tmr < 1020 && m_Sta_count < 52)
	{
		if (m_Sta_count == 51)StationName = 0;
		else if (StationName == m_Sta_No * 2 + 1)
		{
			m_Sta_count += 1;
			StationName = m_Sta_No * 2 + 2;
			m_Sta_tmr += *DeltaT;
		}
		else
		{
			StationName = m_Sta_No * 2 + 2;
			m_Sta_tmr += *DeltaT;
		}
	}
	else if (m_halt && m_Sta_count < 51)
	{
		StationName = m_Sta_No * 2 + 2;
		m_Sta_tmr %= 1020;
	}
	else
	{
		StationName = 0;
		m_Sta_tmr = 0;
	}
}
void CATSPN::haltOFF(void)
{
	m_halt = false;
	m_halt_P = false;
	m_halt_b = false;
	m_halt_App = false;
	m_haltchime = ATS_SOUND_STOP;
}
// 線区最高速度
void CATSPN::LineMax(int speed)
{
	m_Line_Max_Speed = float(speed);
}
// 速度制限区間
void CATSPN::LimitSpeedON(int param)
{
	m_LimitSpeed = true;
	m_LimitSpeed_Speed = float((param % 1000) + 1);
	m_LimitSpeed_dist = float(param / 1000);
}
void CATSPN::LimitSpeed()
{
	if (m_LimitSpeed)
	{
		float pattern = (*TrainSpeed * *TrainSpeed - m_LimitSpeed_Speed * m_LimitSpeed_Speed) / PN_DECELERATION;//減速に必要な距離
		float Approach = (*TrainSpeed * *TrainSpeed - m_LimitSpeed_Speed * m_LimitSpeed_Speed) / PN_APPROACH;
		float def = *TrainSpeed / 3600.0f * *DeltaT;//1フレームで進んだ距離[m]
		m_LimitSpeed_dist -= def;
		if (m_LimitSpeed_dist >= 0)//制限速度に入るまで
		{
			m_LimitSpeed_App = (Approach >= m_LimitSpeed_dist);
			m_LimitSpeed_b = (pattern >= m_LimitSpeed_dist);
		}
		else//速度制限区間に入ってから
		{
			m_LimitSpeed_App = (*TrainSpeed > m_LimitSpeed_Speed);
			m_LimitSpeed_b = (*TrainSpeed > m_LimitSpeed_Speed);
		}
	}
}
void CATSPN::LimitSpeedOFF(void)
{
	m_LimitSpeed = false;
	m_LimitSpeed_b = false;
	m_LimitSpeed_App = false;
}
// 終端防護（非常指令）
void CATSPN::TerminalSafetyON(int dist)
{
	m_TerminalSafety = true;
	m_Terminal_Dist = float(dist);
}
void CATSPN::TerminalSafety(void)
{
	if (m_TerminalSafety)
	{
		float pattern = *TrainSpeed * *TrainSpeed / PN_DECELERATION;//停止に必要な距離
		float Approach = *TrainSpeed * *TrainSpeed / PN_APPROACH;
		float def = *TrainSpeed / 3600.0f * *DeltaT;//1フレームで進んだ距離[m]
		m_Terminal_Dist -= def;
		m_TerminalSafety_App = (Approach >= m_Terminal_Dist);
		m_TerminalSafety_b = ((pattern >= m_Terminal_Dist && *TrainSpeed > 5) || m_Terminal_Dist <= -0.5);
	}
}