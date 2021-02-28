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
#include "..\..\common\CAtsSound.h"
#include "..\..\common\CAtsSoundLoop.h"

class CATSPN
{
public:
	//�p�^�[���ǂݍ���
	void setparam(std::filesystem::path module_dir, float* pSpeed, int* pDelT, double* pDelL, int* pBpos);
	// ������
	void initATSPN(void)noexcept;
	//���Z�b�g�{�^��
	void resetATSPN(void)noexcept;
	//PN������s
	void RunPNcontrol(void)noexcept;
	// �w�ʖh�~
	void haltON(int)noexcept;
	void stopPattern(int)noexcept;
	void halt(void)noexcept;
	void haltOFF(void)noexcept;
	// ����ō����x
	void LineMax(int)noexcept;
	// ���x�������
	void LimitSpeedON(int)noexcept;
	void LimitSpeed(void)noexcept;
	void LimitSpeedOFF(void)noexcept;
	// �I�[�h��i���w�߁j
	void TerminalSafetyON(int)noexcept;
	void TerminalSafety(void)noexcept;
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
	CAtsSound<6> HaltSound;
	//�p�^�[���ڋ߉���
	CAtsSoundLoop<11> ApproachSound;

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
	float m_LimitSpeed_Speed = std::numeric_limits<float>::max();
	//���x�����J�n����
	double m_LimitSpeed_dist = 0.0;
	//�I�[�h�싗��
	double m_Terminal_Dist = 0.0;
	//����ō����x
	double m_Line_Max_Speed = std::numeric_limits<float>::max();
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
	//���x
	float* m_pTrainSpeed = nullptr;
	//�O��Ƃ̎����̍�
	int* m_pDeltaT = nullptr;
	//�O��Ƃ̈ʒu�̍�
	double* m_pDeltaL = nullptr;
	//�u���[�L�n���h���̈ʒu
	int* m_pBrake = nullptr;
	//�u���[�L�p�^�[��
	std::vector<DISTANCE_SET> m_pattern{};
	//���݂̑��x����̒�~����
	double m_stopDist = 0.0;
	//�������x����̒�~����
	double m_stopDistFromLimit = 0.0;
	//�u���[�L�p�^�[���̍ő呬�x
	float m_pattern_Max = std::numeric_limits<float>::max();
	//�u���[�L�p�^�[���̌덷
	std::vector<DISTANCE_SET> m_ErrPatten{};
	//���݂̌덷
	double m_CurrentErr = 0.0;

	static constexpr float m_deceleration = 3.7f * 7.2f;//�����萔�i�����x[km/h/s] x 7.2�j
	static constexpr float m_approach = m_deceleration * 0.5f;//P�ڋߕ\������
	static constexpr float m_offset = 0.5f;//�i�ԏ�q�I�t�Z�b�g[m]�j
};

#endif _CATSPN_INCLUDED_