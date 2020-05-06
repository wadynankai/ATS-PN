#pragma once
#include "atsplugin.h"

#define PN_DECELERATION 26.64f//�����萔�i�����x[km/h/s] x 7.2�j
#define PN_APPROACH 13.32f
#define PN_OFFSET 0.5f//�i�ԏ�q�I�t�Z�b�g[m]�j
class CATSPN
{
public:
	CATSPN();
	~CATSPN();
	// ������
	void initATSPN(void);
	//���Z�b�g�{�^��
	void resetATSPN(void);
	//PN������s
	void RunPNcontrol(void);
	// �w�ʖh�~
	void haltON(int);
	void stopPattern(int);
	void halt(void);
	void haltOFF(void);
	// ����ō����x
	void LineMax(int);
	// ���x�������
	void LimitSpeedON(int);
	void LimitSpeed(void);
	void LimitSpeedOFF(void);
	// �I�[�h��i���w�߁j
	void TerminalSafetyON(int);
	void TerminalSafety(void);
	//���x
	float* TrainSpeed;
	//�O��Ƃ̎����̍�
	int *DeltaT;
	//���u���[�L�w��
	bool emgBrake;
	//��p�ő�u���[�L�w��
	bool svcBrake;
	//PN����\��
	bool PNcontrolDisp;
	//���x�����\��
	int SpeedLimitDisp;
	//�w�ʖh�~�\��
	int haltDisp;
	//�I�[�h��\��
	int TerminalSafetyDisp;
	//�w���\��
	int StationName;
	//P�ڋ�
	bool PatternApproachDisp;
	//�w�ʖh�~�`���C��
	int HaltSound;
	//�p�^�[���ڋ߉���
	int ApproachSound;

private:
	//PN����i�w�ʖh�~�j
	bool m_halt;
	//PN����i���x�����j
	bool m_LimitSpeed;
	//PN����i�I�[�h��)
	bool m_TerminalSafety;
	//P�ڋ߁i�w�ʖh�~�j
	bool m_halt_App;
	//P�ڋ߁i���x�����j
	bool m_LimitSpeed_App;
	//P�ڋ߁i�I�[�h��j
	bool m_TerminalSafety_App;
	//�w�ԍ�
	int m_Sta_No;
	//�w�ԍ��_�ŃJ�E���^
	int m_Sta_count;
	//�w�ԍ��_�Ń^�C�}�[
	int m_Sta_tmr;
	//�w�ʖh�~��~����
	float m_halt_dist;
	//���x�������x
	float m_LimitSpeed_Speed;
	//���x�����J�n����
	float m_LimitSpeed_dist;
	//�I�[�h�싗��
	float m_Terminal_Dist;
	//����ō����x
	float m_Line_Max_Speed;
	//�w�ʖh�~�`���C���i�w�Ɉړ��΍�j
	int m_haltchime;
	//�w�ʖh�~�`���C���Đ��ς�
	bool m_haltchime_played;
	//�w�ʖh�~�p�^�[������
	bool m_halt_P;
	//����ō����x�u���[�L
	bool m_LineMaxSpeed_b;
	//�w�ʖh�~�u���[�L
	bool m_halt_b;
	//���x�����u���[�L
	bool m_LimitSpeed_b;
	//�I�[�h��u���[�L
	bool m_TerminalSafety_b;

};

