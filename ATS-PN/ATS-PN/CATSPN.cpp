#include "CATSPN.h"

//パターン読み込み
CATSPN::CATSPN(std::filesystem::path& module_dir, float& Speed, std::chrono::milliseconds& DelT, double& DelL, int& Bpos) : 
	m_TrainSpeed{ Speed }, m_DeltaT{ DelT }, m_DeltaL{ DelL }, m_Brake{ Bpos }
{
	makeTableFromCsv(module_dir / L"AtsPnPattern.csv", m_pattern, 1, std::locale(".UTF-8"));
	if (!m_pattern.empty())m_pattern_Max = m_pattern.back().first;
	makeTableFromCsv(module_dir / L"AtsPnErrPattern.csv", m_ErrPatten, 1, std::locale(".UTF-8"));
	if (m_ErrPatten.empty())m_ErrPatten.emplace_back(0.0f, 0.0);
}
// 初期化
void CATSPN::initATSPN(void)noexcept
{
	haltOFF();
	LimitSpeedOFF();
	if(m_LimitSpeed_Speed==std::numeric_limits<float>::max())
	{
		m_LimitSpeed = false;
		m_LimitSpeed_b = false;
		m_LimitSpeed_emg = false;
		m_LimitSpeed_App = false;
		m_defeatDistance = std::numeric_limits<double>::max();
	}
	m_TerminalSafety = false;
	m_TerminalSafety_b = false;
	m_TerminalSafety_emg = false;
}
// リセットボタン
void CATSPN::resetATSPN(void)noexcept
{
	haltOFF();
	LimitSpeedOFF();
	m_LimitSpeed = false;
	m_LimitSpeed_b = false;
	m_LimitSpeed_emg = false;
	m_LimitSpeed_App = false;
	m_defeatDistance = std::numeric_limits<double>::max();
	m_TerminalSafety = false;
	m_TerminalSafety_b = false;
	m_TerminalSafety_emg = false;

}
//編成長の設定
/*void CATSPN::setFormationLength(int cars) noexcept
{
	m_formationLength = 20.0 * cars;
}*/
//PN制御実行
void CATSPN::RunPNcontrol(void)noexcept
{
	//パターンの計算
	m_LineMaxSpeed_b = (m_TrainSpeed > m_Line_Max_Speed);//線区最高速度を超えたらブレーキ
	m_LineMaxSpeed_emg = (m_TrainSpeed > m_Line_Max_Speed + 5);//線区最高速度を5キロ超えたら非常ブレーキ
	halt();
	LimitSpeed();
	TerminalSafety();
	//ブレーキの動作
	svcBrake = (m_halt_b || m_LimitSpeed_b || m_LineMaxSpeed_b || m_TerminalSafety_b);
	emgBrake = (m_halt_emg || m_LimitSpeed_emg || m_LineMaxSpeed_emg || m_TerminalSafety_emg);

	//表示
	PNcontrolDisp = (m_halt || m_LimitSpeed || m_TerminalSafety || m_LineMaxSpeed_b);//PN制御
	PatternApproachDisp = (m_halt_App || m_LimitSpeed_App || m_TerminalSafety_App || m_LineMaxSpeed_b);//P接近
	//P接近音声
	if (PatternApproachDisp)ApproachSound.SetVolume(1.0f);
	else ApproachSound.SetVolume(0.0f);

	if (m_halt && m_halt_App) haltDisp = 2;//駅通防止赤
	else if (m_halt)haltDisp = 1;//駅通防止白
	else haltDisp = 0;//駅通防止消去

	if ((m_LimitSpeed && m_LimitSpeed_App)|| m_LineMaxSpeed_b)SpeedLimitDisp = 2;//速度制限赤
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
	m_Sta_count = 0ms;//点滅カウンタリセット
	m_Sta_tmr = 0ms;//点滅タイマーリセット
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
		double Approach = pattern + m_TrainSpeed * 5.0f / 3.6f;//常用パターンの5秒分の距離手前でP接近
		if (m_halt_P)
		{
			m_halt_dist -= m_DeltaL;
			m_halt_App = (Approach >= m_halt_dist);
			if (!m_pattern.empty() && m_TrainSpeed <= m_pattern_Max)
			{
				if (!m_halt_b)//ブレーキをかける
				{
					if (pattern >= m_halt_dist && m_TrainSpeed > 10.0f)m_halt_b = true;
				}
				else //常用パターン中
				{
					if (m_TrainSpeed < interpolationInv(m_halt_dist, m_pattern) - 2.0f || m_TrainSpeed <= 10.0f)m_halt_b = false;//ブレーキを解除
					float emgPattern = interpolationInv(m_halt_dist + 10.0, m_pattern) + 5.0f;
					if (m_TrainSpeed > emgPattern)m_halt_emg = true;//非常ブレーキ
				}
			}
			else m_halt_b = (pattern >= m_halt_dist && m_TrainSpeed > 10.0f);
		}
	}
	else
	{
		m_halt_P = false;
		m_halt_b = false;
		m_halt_App = false;
	}
	//駅名点滅
	if (m_halt && m_Sta_tmr.count() >= 0 && m_Sta_tmr.count() <= 510 && m_Sta_count.count() < 52)
	{
		if (StationName == 0 || StationName == m_Sta_No * 4 + 2) m_Sta_count += 1ms;
		StationName = m_Sta_No * 4 + 1;
		m_Sta_tmr += m_DeltaT;
	}
	else if (m_halt && m_Sta_tmr > 510ms && m_Sta_tmr < 1020ms && m_Sta_count < 52ms)
	{
		if (m_Sta_count == 51ms)StationName = 0;
		else if (StationName == m_Sta_No * 4 + 1)
		{
			m_Sta_count += 1ms;
			StationName = m_Sta_No * 4 + 2;
			m_Sta_tmr += m_DeltaT;
		}
		else
		{
			StationName = m_Sta_No * 4 + 2;
			m_Sta_tmr += m_DeltaT;
		}
	}
	else if (m_halt && m_Sta_count < 51ms)
	{
		StationName = m_Sta_No * 4 + 2;
		m_Sta_tmr %= 1020;
	}
	else
	{
		StationName = 0;
		m_Sta_tmr = 0ms;
	}
}
void CATSPN::haltOFF(void)noexcept
{
	m_halt = false;
	m_halt_P = false;
	m_halt_b = false;
	m_halt_emg = false;
	m_halt_App = false;
	m_TerminalSafety = false;
	m_TerminalSafety_b = false;
	m_TerminalSafety_emg = false;
	m_Terminal_Dist = 0.0f;
	HaltSound.Stop();
}
// 線区最高速度
void CATSPN::LineMax(int speed)noexcept
{
	m_Line_Max_Speed = static_cast<float>(speed);
}
// 速度制限区間
void CATSPN::LimitSpeedON(int param) noexcept
{
	m_LimitSpeed = true;
	if (m_LimitSpeed_Speed != std::numeric_limits<float>::max())
	{
		m_LimitSpeed_Speed_pre = m_LimitSpeed_Speed;
		if (m_LimitSpeed_Speed_pre < m_LimitSpeed_Speed) m_defeatDistance = 0.0;//今回の制限の方が高いときは前の制限から最後尾が抜けるまで待つ。
		else m_defeatDistance = m_formationLength;
	}
	m_LimitSpeed_Speed = static_cast<float>((param % 1000) + 1);
	m_LimitSpeed_dist = static_cast<float>(param / 1000);
	if (!m_pattern.empty() && m_LimitSpeed_Speed <= m_pattern_Max)m_stopDistFromLimit = interpolation(m_LimitSpeed_Speed, m_pattern);
	else m_stopDistFromLimit = m_LimitSpeed_Speed * m_LimitSpeed_Speed / m_deceleration;
}
void CATSPN::LimitSpeed()noexcept
{
	if (m_LimitSpeed)
	{
		double err = std::min(m_CurrentErr, m_LimitSpeed_dist - 1);
		double pattern = m_stopDist - m_stopDistFromLimit + err;//減速に必要な距離
		double Approach = pattern + m_TrainSpeed * 5.0f / 3.6f;//常用パターンの5秒分の距離手前でP接近
		m_LimitSpeed_dist -= m_DeltaL;
		//制限解除時の編成長抜けるまでの動作
		if (m_defeatDistance <= m_formationLength)
		{
			if (m_defeatDistance < 0.0)m_defeatDistance = 0.0f;
			m_defeatDistance += m_DeltaL;
			m_LimitSpeed_App = (m_TrainSpeed > m_LimitSpeed_Speed_pre);
			if (!m_LimitSpeed_b)//ブレーキをかける
			{
				if ((m_TrainSpeed > m_LimitSpeed_Speed_pre + 1.0f))m_LimitSpeed_b = true;
				if (m_LimitSpeed_Speed != std::numeric_limits<float>::max())//パターン解除ではない場合
				{
					m_LimitSpeed_App |= (Approach >= m_LimitSpeed_dist && m_TrainSpeed > m_LimitSpeed_Speed + 1.0f);//次の制限速度の接近も考える。
					if ((pattern >= m_LimitSpeed_dist))m_defeatDistance = m_formationLength;//常用パターンに当たったら編成長を抜けた後の動作をする→ブレーキ動作
				}
			}
			else if (m_TrainSpeed < m_LimitSpeed_Speed_pre - 2.0f)m_LimitSpeed_b = false;
		}
		else if (m_defeatDistance < std::numeric_limits<double>::max())//編成が制限を抜けた。
		{
			if (m_LimitSpeed_Speed == std::numeric_limits<float>::max())//制限解除のとき。それ以外は次の制限に移る。
			{
				m_LimitSpeed = false;
				m_LimitSpeed_b = false;
				m_LimitSpeed_emg = false;
				m_LimitSpeed_App = false;
				m_LimitSpeed_Speed_pre = std::numeric_limits<float>::max();
			}
			m_defeatDistance = std::numeric_limits<double>::max();
		}
		else
		{
			if (m_LimitSpeed_dist >= 0)//制限速度に入るまで
			{
				m_LimitSpeed_App = (Approach >= m_LimitSpeed_dist && m_TrainSpeed > m_LimitSpeed_Speed + 1.0f);
				if (!m_pattern.empty() && m_TrainSpeed <= m_pattern_Max)
				{
					if (!m_LimitSpeed_b)//ブレーキをかける
					{
						if ((pattern >= m_LimitSpeed_dist))m_LimitSpeed_b = true;
					}
					else //常用パターン中
					{
						if (m_TrainSpeed < interpolationInv(m_LimitSpeed_dist + m_stopDistFromLimit, m_pattern) - 2.0f)m_LimitSpeed_b = false;//ブレーキを解除
						float emgPattern = interpolationInv(m_LimitSpeed_dist + m_stopDistFromLimit + 10.0, m_pattern) + 5.0f;
						if (m_TrainSpeed > emgPattern)m_LimitSpeed_emg = true;//非常ブレーキ
					}
				}
				else m_LimitSpeed_b = (pattern >= m_LimitSpeed_dist);
			}
			else//速度制限区間に入ってから
			{
				m_LimitSpeed_App = (m_TrainSpeed > m_LimitSpeed_Speed);
				if (!m_LimitSpeed_b)//ブレーキをかける
				{
					if ((m_TrainSpeed > m_LimitSpeed_Speed + 1.0f))m_LimitSpeed_b = true;
				}
				else if (m_TrainSpeed < m_LimitSpeed_Speed - 2.0f)m_LimitSpeed_b = false;
			}
		}		
	}
}
void CATSPN::LimitSpeedOFF(void)noexcept
{
	m_defeatDistance = 0.0;
	m_LimitSpeed_Speed_pre = m_LimitSpeed_Speed;
	m_LimitSpeed_Speed = std::numeric_limits<float>::max();
}
// 終端防護（非常指令）
void CATSPN::TerminalSafetyON(int dist)noexcept
{
	m_TerminalSafety = true;
	m_Terminal_Dist = static_cast<float>(dist);
}
void CATSPN::TerminalSafety(void)noexcept
{
	if (m_TerminalSafety)
	{
		double pattern = m_stopDist + m_CurrentErr;//停止に必要な距離
		double Approach = pattern + m_TrainSpeed * 5.0f / 3.6f;//常用パターンの5秒分の距離手前でP接近
		m_Terminal_Dist -= m_DeltaL;
		m_TerminalSafety_App = (Approach >= m_Terminal_Dist);
		if (!m_pattern.empty() && m_TrainSpeed <= m_pattern_Max)
		{
			if (!m_TerminalSafety_b)//ブレーキをかける
			{
				if ((pattern >= m_Terminal_Dist && m_TrainSpeed > 7.5f) || m_Terminal_Dist <= -0.5)m_TerminalSafety_b = true;
			}
			else //常用パターン中
			{
				if (m_TrainSpeed < interpolationInv(m_Terminal_Dist, m_pattern) - 2.0f || m_TrainSpeed <= 7.5f)m_TerminalSafety_b = false;//ブレーキを解除
				if (m_TrainSpeed > interpolationInv(m_Terminal_Dist + 10.0, m_pattern) + 5.0f)m_TerminalSafety_emg = true;//非常ブレーキ
			}
		}
		else m_TerminalSafety_b = ((pattern >= m_Terminal_Dist && m_TrainSpeed > 7.5f) || m_Terminal_Dist <= -0.5);
	}
}