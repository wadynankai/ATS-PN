#include "stdafx.h"
#include "CATSPN.h"


CATSPN::CATSPN()
{
}


CATSPN::~CATSPN()
{
}


// ������
void CATSPN::initATSPN(void)
{
	m_Line_Max_Speed = 999;
	haltOFF();
	LimitSpeedOFF();
	m_TerminalSafety = false;
	m_TerminalSafety_b = false;
}
// ���Z�b�g�{�^��
void CATSPN::resetATSPN(void)
{
	haltOFF();
	LimitSpeedOFF();
	m_TerminalSafety = false;
	m_TerminalSafety_b = false;
}
//PN������s
void CATSPN::RunPNcontrol(void)
{
	//�p�^�[���̌v�Z
	m_LineMaxSpeed_b = (*TrainSpeed > m_Line_Max_Speed);//����ō����x�𒴂�����u���[�L
	halt();
	LimitSpeed();
	TerminalSafety();
	//�u���[�L�̓���
	svcBrake = (m_halt_b || m_LimitSpeed_b || m_LineMaxSpeed_b);
	emgBrake = m_TerminalSafety_b;

	//�\��
	PNcontrolDisp = (m_halt || m_LimitSpeed || m_TerminalSafety);//PN����
	PatternApproachDisp = (m_halt_App || m_LimitSpeed_App || m_TerminalSafety_App);//P�ڋ�
	//P�ڋ߉���
	if (PatternApproachDisp)ApproachSound = ATS_SOUND_PLAYLOOPING;
	else ApproachSound = ATS_SOUND_STOP;

	if (m_halt && m_halt_App) haltDisp = 2;//�w�ʖh�~��
	else if (m_halt)haltDisp = 1;//�w�ʖh�~��
	else haltDisp = 0;//�w�ʖh�~����

	if (m_LimitSpeed && m_LimitSpeed_App)SpeedLimitDisp = 2;//���x������
	else if (m_LimitSpeed)SpeedLimitDisp = 1;//���x������
	else SpeedLimitDisp = 0;//���x��������

	if (m_TerminalSafety && m_TerminalSafety_App)TerminalSafetyDisp = 2;//�I�[�h���
	else if (m_TerminalSafety)TerminalSafetyDisp = 1;//�I�[�h�씒
	else TerminalSafetyDisp = 0;//�I�[�h�����
}
// �w�ʖh�~
void CATSPN::haltON(int number)
{
	m_halt = true;
	m_Sta_No = number;
	m_haltchime_played = false;
	m_Sta_count = 0;//�_�ŃJ�E���^���Z�b�g
	m_Sta_tmr = 0;//�_�Ń^�C�}�[���Z�b�g
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
		float pattern = *TrainSpeed * *TrainSpeed / PN_DECELERATION;//��~�ɕK�v�ȋ���
		float Approach = *TrainSpeed * *TrainSpeed / PN_APPROACH;
		if (!m_haltchime_played)
		{
			m_haltchime = ATS_SOUND_PLAY;
			m_haltchime_played = true;
		}
		if (m_halt_P)
		{
			float def = *TrainSpeed / 3600.0f * *DeltaT;//1�t���[���Ői�񂾋���[m]
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
	//�w���_��
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
// ����ō����x
void CATSPN::LineMax(int speed)
{
	m_Line_Max_Speed = float(speed);
}
// ���x�������
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
		float pattern = (*TrainSpeed * *TrainSpeed - m_LimitSpeed_Speed * m_LimitSpeed_Speed) / PN_DECELERATION;//�����ɕK�v�ȋ���
		float Approach = (*TrainSpeed * *TrainSpeed - m_LimitSpeed_Speed * m_LimitSpeed_Speed) / PN_APPROACH;
		float def = *TrainSpeed / 3600.0f * *DeltaT;//1�t���[���Ői�񂾋���[m]
		m_LimitSpeed_dist -= def;
		if (m_LimitSpeed_dist >= 0)//�������x�ɓ���܂�
		{
			m_LimitSpeed_App = (Approach >= m_LimitSpeed_dist);
			m_LimitSpeed_b = (pattern >= m_LimitSpeed_dist);
		}
		else//���x������Ԃɓ����Ă���
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
// �I�[�h��i���w�߁j
void CATSPN::TerminalSafetyON(int dist)
{
	m_TerminalSafety = true;
	m_Terminal_Dist = float(dist);
}
void CATSPN::TerminalSafety(void)
{
	if (m_TerminalSafety)
	{
		float pattern = *TrainSpeed * *TrainSpeed / PN_DECELERATION;//��~�ɕK�v�ȋ���
		float Approach = *TrainSpeed * *TrainSpeed / PN_APPROACH;
		float def = *TrainSpeed / 3600.0f * *DeltaT;//1�t���[���Ői�񂾋���[m]
		m_Terminal_Dist -= def;
		m_TerminalSafety_App = (Approach >= m_Terminal_Dist);
		m_TerminalSafety_b = ((pattern >= m_Terminal_Dist && *TrainSpeed > 5) || m_Terminal_Dist <= -0.5);
	}
}