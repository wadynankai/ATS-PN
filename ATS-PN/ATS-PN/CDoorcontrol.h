#pragma once
#include "stdafx.h"
#include "atsplugin.h"
#include "CSourceVoice.h"
#include <vector>
#include <string>
#include <fstream>

#define DOOR_TIMER_OPN 7000//�򍲖�ŉE���̔����J���܂ł̎���[ms]
#define DOOR_TIMER_CLS2 35000//�򍲖��2(3)�Ԃ̂�Γ������E���̔����܂�܂ł̎���[ms]
#define DOOR_TIMER_CLS5 20000//�򍲖��5(4)�Ԃ̂�Γ������E���̔����܂�܂ł̎���[ms]
#define DOOR_TIMER_NAMBA 20000//��g�ŏ�ԑ��̔����J���Ă���~�ԑ��̔�������܂ł̎���[ms]

class CDoorcontrol
{
public:
	CDoorcontrol();
	CDoorcontrol(std::wstring, int, IXAudio2* pXau2 = nullptr);
	~CDoorcontrol();
	void setTrainNo(int);
	void Running(ATS_VEHICLESTATE&);
	void NextSano(void);
	void NextNamba(void);
	void DoorOpn(void);
	void DoorCls(void);
	void NambaDoorOpn(void);
	int doorYama = 0;//�h�A�R�C���W�P�[�^�\(0:����,1:����,2:�_��)
	int doorUmi = 0;//�h�A�C�C���W�P�[�^�\(0:����,1:����,2:�_��)
	int DoorClsL = ATS_SOUND_CONTINUE, DoorClsR = ATS_SOUND_CONTINUE, DoorOpnL = ATS_SOUND_CONTINUE, DoorOpnR = ATS_SOUND_CONTINUE;//�h�A�J��

private:
	void init(void);
	void loadconfig(void);
	bool m_pilotLampL = true, m_pilotLampR = true;
	std::wstring m_module_dir;
	std::wstring m_tableFileName;
	bool m_pilotLamp;//BVE�{�̂̃h�A�u�v����true
	int m_trainNo = 0;
	bool m_sanoF, m_nambaF;//���̉w���򍲖���g�ł��邱�Ƃ������t���O
	int m_sanoOpnTimer = 0, m_sanoClsTimer = 0, m_nambaOpnTimer2 = 0;
	int m_OpenTime = 0, m_OpenTime_pre = 0;//�h�A���J���Ă���̎���
	int m_sanoTrack = 0, m_nambaTrack = 0;//�򍲖�Ɠ�g�̔Ԑ�

	IXAudio2* m_pXAudio2 = nullptr;
	CSourceVoice* m_DoorClsL = nullptr, *m_DoorClsR = nullptr, *m_DoorOpnL = nullptr, *m_DoorOpnR = nullptr;

	//�R�����g��X�y�[�X����������
	void cleanUpBveStr(std::string&);
	void cleanUpBveStr(std::wstring&);
	//csv�̍s�𕪊�����B
	void splitCsvColumn(std::string&, std::vector<std::string>&);
	void splitCsvColumn(std::wstring&, std::vector<std::wstring>&);
	//�啶���Ə���������ʂ��Ȃ�������r(�������Ƃ���true)
	bool icasecmp(const std::string& l, const std::string& r);
	bool icasecmp(const std::wstring& l, const std::wstring& r);

};

