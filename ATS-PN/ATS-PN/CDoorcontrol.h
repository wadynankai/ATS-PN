#ifndef _CDOOR_CONTROL_INCLUDED
#define _CDOOR_CONTROL_INCLUDED

#include <filesystem>
#include <fstream>
#include "..\..\common\LoadBveText.h"
#include "..\..\common\CAudioFileInputNode.h"

namespace DoorTimer
{
	constexpr int open = 7000;//�򍲖�ŉE���̔����J���܂ł̎���[ms]
	constexpr int SanoCls2 = 35000;//�򍲖��2(3)�Ԃ̂�Γ������E���̔����܂�܂ł̎���[ms]
	constexpr int SanoCls5 = 20000;//�򍲖��5(4)�Ԃ̂�Γ������E���̔����܂�܂ł̎���[ms]
	constexpr int NambaCls = 20000;//��g�ŏ�ԑ��̔����J���Ă���~�ԑ��̔�������܂ł̎���[ms]
};

class CDoorcontrol
{
public:
	~CDoorcontrol();
	static void CreateInstance(const std::filesystem::path& moduleDir,
		const winrt::Windows::Media::Audio::AudioGraph& graph = nullptr, const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode = nullptr)
	{
		if (!instance)instance.reset(new CDoorcontrol(moduleDir, graph, outputNode));
	}
/*	static void Delete()noexcept
	{
		instance.reset();
	}*/
	static std::unique_ptr<CDoorcontrol>& GetInstance()noexcept
	{
		return instance;
	}
	void setTrainNo(const int);
	inline void Running(const int)noexcept;
	inline void Halt(const int)noexcept;
	inline void DoorOpn(void)noexcept;
	inline void DoorCls(void)noexcept;
	inline void NambaDoorOpn(void)noexcept;
	int doorYama = 0;//�h�A�R�C���W�P�[�^�\(0:����,1:����,2:�_��)
	int doorUmi = 0;//�h�A�C�C���W�P�[�^�\(0:����,1:����,2:�_��)

private:
	CDoorcontrol() = delete;
	CDoorcontrol(const std::filesystem::path& moduleDir, 
		const winrt::Windows::Media::Audio::AudioGraph& graph = nullptr, const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode = nullptr);
	CDoorcontrol& operator=(CDoorcontrol&) = delete;
	CDoorcontrol& operator=(CDoorcontrol&&) = delete;
	inline static std::unique_ptr<CDoorcontrol> instance;
	void loadconfig(void);
	bool m_pilotLampL = true, m_pilotLampR = true;
	std::filesystem::path m_module_dir;
	std::filesystem::path m_tableFileName;
	bool m_pilotLamp;//BVE�{�̂̃h�A�u�v����true
	int m_trainNo = 0;
	bool m_sanoF, m_nambaF;//���̉w���򍲖���g�ł��邱�Ƃ������t���O
	int m_OpenTime = 0, m_OpenTime_pre = 0;//�h�A���J���Ă���̎���
	int m_sanoTrack = 0, m_nambaTrack = 0;//�򍲖�Ɠ�g�̔Ԑ�

	winrt::Windows::Media::Audio::AudioGraph m_graph;
	winrt::Windows::Media::Audio::AudioDeviceOutputNode m_outputNode;
	bool m_graphCreated = false, m_outputNodeCreated = false;
	CAudioFileInputNode m_DoorClsL = nullptr, m_DoorClsR = nullptr, m_DoorOpnL = nullptr, m_DoorOpnR = nullptr;
};


inline void CDoorcontrol::Running(const int time) noexcept
{
	static int time_pre = 0, deltaT = 0;
	deltaT = time - time_pre;
	if (deltaT < 0 || deltaT >= 1000)deltaT = 0;//�w�Ɉړ��΍�
	if (!m_pilotLamp)m_OpenTime += deltaT;


	if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 2 || m_nambaTrack == 4 || m_nambaTrack == 6 || m_nambaTrack == 8))//��g�����Ԑ�
	{
		if (m_OpenTime_pre < DoorTimer::NambaCls && m_OpenTime >= DoorTimer::NambaCls)
		{
			if (m_DoorClsL)m_DoorClsL.flag = true;
			if (m_DoorClsL)m_DoorClsL.Start();
		}
		if (m_DoorClsL)if (m_DoorClsL.flag && !m_DoorClsL.isRunning())doorUmi = 1, m_DoorClsL.flag = false, m_pilotLampL = true;
	}
	else if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 1 || m_nambaTrack == 3 || m_nambaTrack == 5 || m_nambaTrack == 7))//��g��Ԑ�
	{
		if (m_OpenTime_pre < DoorTimer::NambaCls && m_OpenTime >= DoorTimer::NambaCls)
		{
			if (m_DoorClsR)m_DoorClsR.flag = true;
			if (m_DoorClsR)m_DoorClsR.Start();
		}
		if (m_DoorClsR)if (m_DoorClsR.flag && !m_DoorClsR.isRunning())doorYama = 1, m_DoorClsR.flag = false, m_pilotLampR = true;
	}
	else if (m_sanoF && !m_pilotLamp && (m_sanoTrack == 2 || m_sanoTrack == 3))//�򍲖�2(3)�ԏ���
	{
		if (m_OpenTime_pre < DoorTimer::open && m_OpenTime >= DoorTimer::open)
		{
			if (m_DoorOpnR)m_DoorOpnR.Start();
			doorUmi = 2;
			m_pilotLampR = false;
		}
		if (m_OpenTime_pre < DoorTimer::SanoCls2 && m_OpenTime >= DoorTimer::SanoCls2)
		{
			if (m_DoorClsR)m_DoorClsR.flag = true;
			if (m_DoorClsR)m_DoorClsR.Start();
		}
		if (m_DoorClsR)if (m_DoorClsR.flag && !m_DoorClsR.isRunning())doorUmi = 0, m_DoorClsR.flag = false, m_pilotLampR = true;
	}
	else if (m_sanoF && !m_pilotLamp && (m_sanoTrack == 5 || m_sanoTrack == 4))//�򍲖�5(4)�ԏ���
	{
		if (m_OpenTime_pre < DoorTimer::open && m_OpenTime >= DoorTimer::open)
		{
			if (m_DoorOpnR)m_DoorOpnR.Start();
			doorYama = 2;
			m_pilotLampR = false;
		}
		if (m_OpenTime_pre < DoorTimer::SanoCls5 && m_OpenTime >= DoorTimer::SanoCls5)
		{
			if (m_DoorClsR)m_DoorClsR.flag = true;
			if (m_DoorClsR)m_DoorClsR.Start();
		}
		if (m_DoorClsR)if (m_DoorClsR.flag && !m_DoorClsR.isRunning())doorYama = 0, m_DoorClsR.flag = false, m_pilotLampR = true;
	}
	else
	{
		doorUmi = 0;
		doorYama = 0;
	}
	m_OpenTime_pre = m_OpenTime;
	time_pre = time;
}

inline void CDoorcontrol::Halt(const int staNo)noexcept
{
	switch (staNo)
	{
	case 1:
		m_nambaF = true;
		m_sanoF = false;
		break;
	case 30:
		m_nambaF = false;
		m_sanoF = true;
		break;
	default:
		m_nambaF = false;
		m_sanoF = false;
		break;
	}
}

inline void CDoorcontrol::DoorOpn(void)noexcept
{
	m_pilotLamp = false;
	m_OpenTime = 0;
}

inline void CDoorcontrol::DoorCls(void)noexcept
{
	m_sanoF = false;
	m_nambaF = false;
	m_pilotLamp = true;
	m_pilotLampL = true;
	m_pilotLampR = true;
	m_OpenTime = 0;
	doorUmi = 0;
	doorYama = 0;
}

inline void CDoorcontrol::NambaDoorOpn(void)noexcept
{
	if (m_nambaF && !m_pilotLamp && m_pilotLampR && (m_nambaTrack == 2 || m_nambaTrack == 4 || m_nambaTrack == 6 || m_nambaTrack == 8))//��g�����Ԑ�
	{
		if (m_DoorOpnR)m_DoorOpnR.Start();
		doorYama = 2;
		m_pilotLampR = false;
	}
	else if (m_nambaF && !m_pilotLamp && m_pilotLampL && (m_nambaTrack == 1 || m_nambaTrack == 3 || m_nambaTrack == 5 || m_nambaTrack == 7))//��g��Ԑ�
	{
		if (m_DoorOpnL)m_DoorOpnL.Start();
		doorUmi = 2;
		m_pilotLampL = false;
	}
}


#endif //_CDOOR_CONTROL_INCLUDED