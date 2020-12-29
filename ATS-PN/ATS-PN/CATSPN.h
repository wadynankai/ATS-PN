#ifndef _CATSPN_INCLUDED_
#define _CATSPN_INCLUDED_

#ifndef _WIN32_WINNT
#include <winsdkver.h>
#define _WIN32_WINNT _WIN32_WINNT_WIN10
//#define _WIN32_WINNT _WIN32_WINNT_WINBLUE
//#define _WIN32_WINNT _WIN32_WINNT_WIN8
//#define _WIN32_WINNT _WIN32_WINNT_WIN7
#endif //_WIN32_WINNT
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Windows �w�b�_�[����قƂ�ǎg�p����Ă��Ȃ����������O����
#endif
#define NOMINMAX
#include <Windows.h>
#include "atsplugin.h"
#include <algorithm>
#include <vector>
#include <limits>
#include <filesystem>
#include "../..\common/CURRENT_SET.h"

class CATSPN
{
public:
	//�p�^�[���ǂݍ���
	void loadPattern(std::filesystem::path module_dir);
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
	float* TrainSpeed = nullptr;
	//�O��Ƃ̎����̍�
	int* DeltaT = nullptr;
	//�O��Ƃ̈ʒu�̍�
	double* DeltaL = nullptr;
	//���u���[�L�w��
	bool emgBrake = false;
	//��p�ő�u���[�L�w��
	bool svcBrake = false;
	//PN����\��
	bool PNcontrolDisp = false;
	//���x�����\��
	int SpeedLimitDisp = 0;
	//�w�ʖh�~�\��
	int haltDisp = 0;
	//�I�[�h��\��
	int TerminalSafetyDisp = 0;
	//�w���\��
	int StationName = 0;
	//P�ڋ�
	bool PatternApproachDisp = false;
	//�w�ʖh�~�`���C��
	int HaltSound = 0;
	//�p�^�[���ڋ߉���
	int ApproachSound = ATS_SOUND_STOP;

private:
	//PN����i�w�ʖh�~�j
	bool m_halt = false;
	//PN����i���x�����j
	bool m_LimitSpeed = false;
	//PN����i�I�[�h��)
	bool m_TerminalSafety = false;
	//P�ڋ߁i�w�ʖh�~�j
	bool m_halt_App = false;
	//P�ڋ߁i���x�����j
	bool m_LimitSpeed_App = false;
	//P�ڋ߁i�I�[�h��j
	bool m_TerminalSafety_App = false;
	//�w�ԍ�
	int m_Sta_No = 0;
	//�w�ԍ��_�ŃJ�E���^
	int m_Sta_count = 0;
	//�w�ԍ��_�Ń^�C�}�[
	int m_Sta_tmr = 0;
	//�w�ʖh�~��~����
	double m_halt_dist = 0.0;
	//���x�������x
	float m_LimitSpeed_Speed = 0.0f;
	//���x�����J�n����
	double m_LimitSpeed_dist = 0.0;
	//�I�[�h�싗��
	double m_Terminal_Dist = 0.0;
	//����ō����x
	double m_Line_Max_Speed = 0.0;
	//�w�ʖh�~�`���C���i�w�Ɉړ��΍�j
//	int m_haltchime;
	//�w�ʖh�~�`���C���Đ��ς�
	bool m_haltchime_played = false;
	//�w�ʖh�~�p�^�[������
	bool m_halt_P = false;
	//����ō����x�u���[�L
	bool m_LineMaxSpeed_b = false;
	//�w�ʖh�~�u���[�L
	bool m_halt_b = false;
	//���x�����u���[�L
	bool m_LimitSpeed_b = false;
	//�I�[�h��u���[�L
	bool m_TerminalSafety_b = false;
	//�u���[�L�p�^�[��
	std::vector<DISTANCE_SET> m_pattern;
	//�u���[�L�p�^�[���̍ő呬�x
	float m_pattern_Max = 0.0f;
	//�u���[�L�p�^�[���̌덷
	std::vector<DISTANCE_SET> m_ErrPatten;

	static constexpr float m_deceleration = 3.7f * 7.2f;//�����萔�i�����x[km/h/s] x 7.2�j
	static constexpr float m_approach = m_deceleration * 0.5f;//P�ڋߕ\������
	static constexpr float m_offset = 0.5f;//�i�ԏ�q�I�t�Z�b�g[m]�j
};

#endif _CATSPN_INCLUDED_