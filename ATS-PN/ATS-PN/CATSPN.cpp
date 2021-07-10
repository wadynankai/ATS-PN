#include "CATSPN.h"

//パターン読み込み
CATSPN::CATSPN(std::filesystem::path& module_dir, float& Speed, int& DelT, double& DelL, int& Bpos) : 
	m_TrainSpeed{ Speed }, m_DeltaT{ DelT }, m_DeltaL{ DelL }, m_Brake{ Bpos }
{
	makeTableFromCsv(module_dir / L"AtsPnPattern.csv", m_pattern);
	if (!m_pattern.empty())m_pattern_Max = m_pattern.back().first;
	makeTableFromCsv(module_dir / L"AtsPnErrPattern.csv", m_ErrPatten);
	if (m_ErrPatten.empty())m_ErrPatten.emplace_back(0.0f, 0.0);
}
// 初期化
void CATSPN::initATSPN(void)noexcept
{
	m_Line_Max_Speed = (std::numeric_limits<int>::max)();
	haltOFF();
	LimitSpeedOFF();
	m_TerminalSafety = false;
	m_TerminalSafety_b = false;
}
// リセットボタン
void CATSPN::resetATSPN(void)noexcept
{
	haltOFF();
	LimitSpeedOFF();
	m_TerminalSafety = false;
	m_TerminalSafety_b = false;

}
//PN制御実行
void CATSPN::RunPNcontrol(void)noexcept
{
	//パターンの計算
	m_LineMaxSpeed_b = (m_TrainSpeed > m_Line_Max_Speed);//線区最高速度を超えたらブレーキ
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
	if (PatternApproachDisp)ApproachSound.SetVolume(1.0f);
	else ApproachSound.SetVolume(0.0f);

	if (m_halt && m_halt_App) haltDisp = 2;//駅通防止赤
	else if (m_halt)haltDisp = 1;//駅通防止白
	else haltDisp = 0;//駅通防止消去

	if (m_LimitSpeed && m_LimitSpeed_App)SpeedLimitDisp = 2;//速度制限赤
	else if (m_LimitSpeed)SpeedLimitDisp = 1;//速度制限白
	else SpeedLimitDisp = 0;//速度制限消去

	if (m_TerminalSafety && m_TerminalSafety_App)TerminalSafetyDisp = 2;//終端防護赤
	else if (m_TerminalSafety)TerminalSafetyDisp = 1;//終端防護白
	else TerminalSafetyDisp = 0;//終端防護消去

	//パターンの計算
	static double brakeDist = 0.0;
	if (PNcontrolDisp)
	{
		if (!m_pattern.empty() && m_TrainSpeed <= m_pattern_Max)
		{
			m_stopDist = interpolation(m_TrainSpeed, m_pattern);
		}
		else
		{
			m_stopDist = m_TrainSpeed * m_TrainSpeed / m_deceleration;
		}

		//パターン誤差の計算
		if (svcBrake || emgBrake || m_Brake > 0)
		{
			brakeDist += m_DeltaL;
		}
		else
		{
			brakeDist = 0.0;
		}

		if (!svcBrake && !emgBrake && m_Brake == 0)
		{
			m_CurrentErr = interpolation(m_TrainSpeed, m_ErrPatten);
		}
		else if (brakeDist > m_CurrentErr)
		{
			if (m_CurrentErr > 0)
			{
				m_CurrentErr -= m_DeltaL;
			}
			else
			{
				m_CurrentErr = 0.0;
			}
		}
	}
	else
	{
		m_stopDist = 0.0;
		m_CurrentErr = 0.0;
		brakeDist = 0.0;
	}

}
// 駅通防止
void CATSPN::haltON(int number)noexcept
{
	m_halt = true;
	m_Sta_No = number;
	HaltSound.Start();
	m_Sta_count = 0;//点滅カウンタリセット
	m_Sta_tmr = 0;//点滅タイマーリセット
}
void CATSPN::stopPattern(int dist)noexcept
{
	if (!m_halt)haltON(0);
	m_halt_P = true;
	m_halt_dist = float(dist);
}
void CATSPN::halt(void)noexcept
{
	if (m_halt)
	{
		double pattern = m_stopDist + m_CurrentErr;//停止に必要な距離
		double Approach = m_TrainSpeed * m_TrainSpeed / m_approach;
		if (m_halt_P)
		{
//			float def = *m_pTrainSpeed / 3600.0f * *m_pDeltaT;//1フレームで進んだ距離[m]
			m_halt_dist -= m_DeltaL;
			m_halt_App = (Approach >= m_halt_dist);
			m_halt_b = (pattern >= m_halt_dist && m_TrainSpeed > 15.0f);
		}
	}
	else
	{
		m_halt_P = false;
		m_halt_b = false;
		m_halt_App = false;
	}
	//駅名点滅
	if (m_halt && m_Sta_tmr >= 0 && m_Sta_tmr <= 510 && m_Sta_count < 52)
	{
		if (StationName == 0 || StationName == m_Sta_No * 4 + 2) m_Sta_count += 1;
		StationName = m_Sta_No * 4 + 1;
		m_Sta_tmr += m_DeltaT;
	}
	else if (m_halt && m_Sta_tmr > 510 && m_Sta_tmr < 1020 && m_Sta_count < 52)
	{
		if (m_Sta_count == 51)StationName = 0;
		else if (StationName == m_Sta_No * 4 + 1)
		{
			m_Sta_count += 1;
			StationName = m_Sta_No * 4 + 2;
			m_Sta_tmr += m_DeltaT;
		}
		else
		{
			StationName = m_Sta_No * 4 + 2;
			m_Sta_tmr += m_DeltaT;
		}
	}
	else if (m_halt && m_Sta_count < 51)
	{
		StationName = m_Sta_No * 4 + 2;
		m_Sta_tmr %= 1020;
	}
	else
	{
		StationName = 0;
		m_Sta_tmr = 0;
	}
}
void CATSPN::haltOFF(void)noexcept
{
	m_halt = false;
	m_halt_P = false;
	m_halt_b = false;
	m_halt_App = false;
	m_TerminalSafety = false;
	m_Terminal_Dist = 0.0f;
	HaltSound.Stop();
}
// 線区最高速度
void CATSPN::LineMax(int speed)noexcept
{
	m_Line_Max_Speed = float(speed);
}
// 速度制限区間
void CATSPN::LimitSpeedON(int param) noexcept
{
	m_LimitSpeed = true;
	m_LimitSpeed_Speed = float((param % 1000) + 1);
	m_LimitSpeed_dist = float(param / 1000);
	if (!m_pattern.empty() && m_LimitSpeed_Speed <= m_pattern_Max)m_stopDistFromLimit = interpolation(m_LimitSpeed_Speed, m_pattern);
	else m_stopDistFromLimit = m_LimitSpeed_Speed * m_LimitSpeed_Speed / m_deceleration;
}
void CATSPN::LimitSpeed()noexcept
{
	if (m_LimitSpeed)
	{
		m_LimitSpeed_dist -= m_DeltaL;
		double err = std::min(m_CurrentErr, m_LimitSpeed_dist - 1);
		double pattern = m_stopDist - m_stopDistFromLimit + err;//減速に必要な距離
		double Approach = (m_TrainSpeed * m_TrainSpeed - m_LimitSpeed_Speed * m_LimitSpeed_Speed) / m_approach;
		if (m_LimitSpeed_dist >= 0)//制限速度に入るまで
		{
			m_LimitSpeed_App = (Approach >= m_LimitSpeed_dist);
			m_LimitSpeed_b = (pattern >= m_LimitSpeed_dist);
		}
		else//速度制限区間に入ってから
		{
			m_LimitSpeed_App = (m_TrainSpeed > m_LimitSpeed_Speed);
			m_LimitSpeed_b = (m_TrainSpeed > m_LimitSpeed_Speed);
		}
	}
}
void CATSPN::LimitSpeedOFF(void)noexcept
{
	m_LimitSpeed = false;
	m_LimitSpeed_b = false;
	m_LimitSpeed_App = false;
}
// 終端防護（非常指令）
void CATSPN::TerminalSafetyON(int dist)noexcept
{
	m_TerminalSafety = true;
	m_Terminal_Dist = float(dist);
}
void CATSPN::TerminalSafety(void)noexcept
{
	if (m_TerminalSafety)
	{
		double pattern = m_stopDist + m_CurrentErr;
		double Approach = m_TrainSpeed * m_TrainSpeed / m_approach;
		m_Terminal_Dist -= m_DeltaL;
		m_TerminalSafety_App = (Approach >= m_Terminal_Dist);
		m_TerminalSafety_b = ((pattern >= m_Terminal_Dist && m_TrainSpeed > 5) || m_Terminal_Dist <= -0.5);
	}
}