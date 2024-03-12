#ifndef _CDOOR_CONTROL_INCLUDED
#define _CDOOR_CONTROL_INCLUDED

#include <filesystem>
#include <fstream>
#include <thread>
#include "..\..\common\LoadBveText.h"
#include "..\..\common\CSourceVoice.h"

#ifdef EXCEPTION
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Windows �w�b�_�[����قƂ�ǎg�p����Ă��Ȃ����������O����
#endif
#include <windows.h>
#include <source_location>
#endif // EXCEPTION


using namespace std::literals::chrono_literals;

namespace DoorTimer
{
	constexpr std::chrono::milliseconds open = 7000ms;//�򍲖�ŉE���̔����J���܂ł̎���[ms]
	constexpr std::chrono::milliseconds SanoCls2 = 35000ms;//�򍲖��2(3)�Ԃ̂�Γ������E���̔����܂�܂ł̎���[ms]
	constexpr std::chrono::milliseconds SanoCls5 = 20000ms;//�򍲖��5(4)�Ԃ̂�Γ������E���̔����܂�܂ł̎���[ms]
	constexpr std::chrono::milliseconds NambaCls = 20000ms;//��g�ŏ�ԑ��̔����J���Ă���~�ԑ��̔�������܂ł̎���[ms]
};

class CDoorcontrol
{
public:
	~CDoorcontrol();
	static void CreateInstance(const std::filesystem::path& moduleDir, const winrt::com_ptr<IXAudio2>& pXau2 = nullptr)
	{
		if (!instance)instance.reset(new CDoorcontrol(moduleDir, pXau2));
	}
/*	static void Delete()noexcept
	{
		instance.reset();
	}*/
	_NODISCARD static std::unique_ptr<CDoorcontrol>& GetInstance()noexcept
	{
		return instance;
	}
	void setTrainNo(const int);
	inline void Running(const std::chrono::time_point <std::chrono::milliseconds>&)noexcept;
	inline void Halt(const int)noexcept;
	inline void DoorOpn(void)noexcept;
	inline void DoorCls(void)noexcept;
	inline void NambaDoorOpn(void)noexcept;
	inline void NambaDoorOpnK(void)noexcept;//��g�w�ō~�ԑ��̔����J����
	inline void NambaDoorClsK(void)noexcept;//��g�w�ō~�ԑ��̔������
	int doorYama = 0;//�h�A�R�C���W�P�[�^�\(0:����,1:����,2:�_��)
	int doorUmi = 0;//�h�A�C�C���W�P�[�^�\(0:����,1:����,2:�_��)
	_NODISCARD const bool getShashouBell(void)noexcept
	{
		if (m_shashouBell.at(0))
		{
			m_shashouBell.at(0) = false;
			m_shashouBell.at(1) = true;
			m_untenshiBell = false;
			return true;
		}
		else _LIKELY
		{
			return false;
		}
	}

	void untenshiBell(void)noexcept
	{
   		if (m_joushaOpen&&!m_shashouBell.at(1))m_untenshiBell = true;
	}

private:
	CDoorcontrol() = delete;
	CDoorcontrol(const std::filesystem::path& moduleDir, const winrt::com_ptr<IXAudio2>& pXau2 = nullptr);
	CDoorcontrol& operator=(CDoorcontrol&) = delete;
	CDoorcontrol& operator=(CDoorcontrol&&) = delete;
	inline static std::unique_ptr<CDoorcontrol> instance;
	void loadconfig(void);
	bool m_pilotLampL = true, m_pilotLampR = true;
	std::filesystem::path m_module_dir;
	std::filesystem::path m_tableFileName;
	bool m_pilotLamp;//BVE�{�̂̃h�A�u�v����true
	bool m_doorSWOpen = false;//������Ŏԏ��X�C�b�`���u�J�v��Ԃ�true
	int m_trainNo = 0;
	bool m_sanoF, m_nambaF;//���̉w���򍲖���g�ł��邱�Ƃ������t���O
	std::chrono::milliseconds m_OpenTime = 0ms, m_OpenTime_pre = 0ms;//�h�A���J���Ă���̎���
	int m_sanoTrack = 0, m_nambaTrack = 0;//�򍲖�Ɠ�g�̔Ԑ�
	bool m_joushaOpen = false;//������ŏ�ԑ��̔����J������True
	bool m_untenshiBell = false;//������ŏ�ԑ��̔����J���C�^�]�m���x����炵����true
	std::chrono::milliseconds m_nambaShashouBellTimer = 0ms;
	static constexpr std::chrono::milliseconds m_nambaShashouBell = 5000ms;//��g�ŉ^�]�m���x����炵�Ă���ԏ����x����炷�܂ł�[ms]
	std::array<bool, 2> m_shashouBell;//0�Ԃ�true�̃t���[���Ŏԏ����x����炷�B���̌�P�Ԃ�true�ɂ���B

	winrt::com_ptr<IXAudio2> m_pXAudio2;
	bool m_graphCreated = false, m_outputNodeCreated = false;
	CSourceVoice m_DoorClsL = nullptr, m_DoorClsR = nullptr, m_DoorOpnL = nullptr, m_DoorOpnR = nullptr, m_HomeDoorOpn = nullptr;
};


inline void CDoorcontrol::Running(const std::chrono::time_point <std::chrono::milliseconds>& time) noexcept
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	static std::chrono::time_point <std::chrono::milliseconds> time_pre;
	static std::chrono::milliseconds deltaT = 0ms;
	deltaT = time - time_pre;
	if (deltaT < 0ms || deltaT >= 1000ms)deltaT = 0ms;//�w�Ɉړ��΍�
	if (!m_pilotLamp)m_OpenTime += deltaT;


	if (m_untenshiBell)
	{
		m_nambaShashouBellTimer += deltaT;
		if (m_nambaShashouBellTimer > m_nambaShashouBell)m_shashouBell.at(0) = true;;
	}



	if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 6 || m_nambaTrack == 8))//��g�����Ԑ��i��C���j�~�ԑ��̔���߂�
	{
		if (m_OpenTime_pre < DoorTimer::NambaCls && m_OpenTime >= DoorTimer::NambaCls)
		{
			m_DoorClsL.flag = true;
			m_DoorClsL->Start();
		}
		else if (m_OpenTime_pre < DoorTimer::NambaCls + 500ms && m_OpenTime >= DoorTimer::NambaCls + 500ms)//���Ⴍ��
		{
			m_DoorClsL.flag = false;
			float rate = static_cast<float>(m_DoorClsL.Position().count()) / static_cast<float>(m_DoorClsL.Duration().count());
			auto startTime = std::chrono::duration_cast<winrt::Windows::Foundation::TimeSpan>(m_DoorOpnL.Duration() * (1.0f - rate));
			m_DoorOpnL.setPlayLength(startTime, m_DoorOpnL.Duration() - startTime, nullptr, nullptr, 0U);
			m_DoorClsL->Stop();
			m_DoorOpnL->Start();
		}
		else if (m_OpenTime_pre < DoorTimer::NambaCls + 1500ms && m_OpenTime >= DoorTimer::NambaCls + 1500ms)
		{
			m_DoorClsL.flag = true;
			m_DoorClsL->Start();
		}
		if (m_DoorClsL.flag && !m_DoorClsL.isRunning())doorUmi = 1, m_DoorClsL.flag = false, m_pilotLampL = true;
	}
	else if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 5 || m_nambaTrack == 7))//��g��Ԑ��i��C���j�~�ԑ��̔���߂�
	{
		if (m_OpenTime_pre < DoorTimer::NambaCls && m_OpenTime >= DoorTimer::NambaCls)
		{
			m_DoorClsR.flag = true;
			m_DoorClsR->Start();
		}
		else if (m_OpenTime_pre < DoorTimer::NambaCls + 500ms && m_OpenTime >= DoorTimer::NambaCls + 500ms)//���Ⴍ��
		{
			m_DoorClsR.flag = false;
			float rate = static_cast<float>(m_DoorClsR.Position().count()) / static_cast<float>(m_DoorClsR.Duration().count());
			auto startTime = std::chrono::duration_cast<winrt::Windows::Foundation::TimeSpan>(m_DoorOpnR.Duration() * (1.0f - rate));
			m_DoorOpnR.setPlayLength(startTime, m_DoorOpnR.Duration() - startTime, nullptr, nullptr, 0U);
			m_DoorClsR->Stop();
			m_DoorOpnR->Start();
		}
		if (m_OpenTime_pre < DoorTimer::NambaCls + 1500ms && m_OpenTime >= DoorTimer::NambaCls + 1500ms)
		{
			m_DoorClsR.flag = true;
			m_DoorClsR->Start();
		}
		if (m_DoorClsR.flag && !m_DoorClsR.isRunning())doorYama = 1, m_DoorClsR.flag = false, m_pilotLampR = true;
	}
	else if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 2 || m_nambaTrack == 4))//��g�����Ԑ��i������j
	{
		if (m_DoorClsL.flag && !m_DoorClsL.isRunning())doorUmi = 1, m_DoorClsL.flag = false, m_pilotLampL = true;
	}
	else if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 1 || m_nambaTrack == 3))//��g��Ԑ��i������j
	{
		if (m_DoorClsR.flag && !m_DoorClsR.isRunning())doorYama = 1, m_DoorClsR.flag = false, m_pilotLampR = true;
	}
	else if (m_sanoF && !m_pilotLamp && (m_sanoTrack == 2 || m_sanoTrack == 3))//�򍲖�2(3)�ԏ���
	{
		if (m_OpenTime_pre < DoorTimer::open && m_OpenTime >= DoorTimer::open)
		{
			m_DoorOpnR->Start();
			doorUmi = 2;
			m_pilotLampR = false;
		}
		if (m_OpenTime_pre < DoorTimer::SanoCls2 && m_OpenTime >= DoorTimer::SanoCls2)
		{
			m_DoorClsR.flag = true;
			m_DoorClsR->Start();
		}
		if (m_DoorClsR.flag && !m_DoorClsR.isRunning())doorUmi = 0, m_DoorClsR.flag = false, m_pilotLampR = true;
	}
	else if (m_sanoF && !m_pilotLamp && (m_sanoTrack == 5 || m_sanoTrack == 4))//�򍲖�5(4)�ԏ���
	{
		if (m_OpenTime_pre < DoorTimer::open && m_OpenTime >= DoorTimer::open)
		{
			m_DoorOpnR->Start();
			doorYama = 2;
			m_pilotLampR = false;
		}
		if (m_OpenTime_pre < DoorTimer::SanoCls5 && m_OpenTime >= DoorTimer::SanoCls5)
		{
			m_DoorClsR.flag = true;
			m_DoorClsR->Start();
		}
		if (m_DoorClsR.flag && !m_DoorClsR.isRunning())doorYama = 0, m_DoorClsR.flag = false, m_pilotLampR = true;
	}
	else
	{
		doorUmi = 0;
		doorYama = 0;
	}
	m_OpenTime_pre = m_OpenTime;
	time_pre = time;
#ifdef EXCEPTION
	}
	catch (std::exception& ex)
	{
		MessageBoxA(nullptr, ex.what(), std::source_location::current().function_name(), MB_OK);
	}
	catch (winrt::hresult_error& hr)
	{
		std::wstringstream ss1, ss2;
		ss1 << L"�G���[�R�[�h�F" << std::hex << hr.code() << L"\n" << hr.message().c_str();
		ss2 << std::source_location::current().function_name();
		MessageBox(nullptr, ss1.str().c_str(), ss2.str().c_str(), MB_OK);
	}
#endif // EXCEPTION
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

inline void CDoorcontrol::DoorOpn(void)noexcept//�ԏ��������J����
{
	m_pilotLamp = false;
	m_doorSWOpen = false;
	m_OpenTime = 0ms;
	m_joushaOpen = false;
	m_untenshiBell = false;
	for (auto& e : m_shashouBell)e = false;
	m_nambaShashouBellTimer = 0ms;
	if (m_nambaF && (m_nambaTrack == 2 || m_nambaTrack == 4 || m_nambaTrack == 6 || m_nambaTrack == 8))//��g�����Ԑ�
	{
		m_pilotLampL = false;
		doorUmi = 2;
	}
	if (m_nambaF && (m_nambaTrack == 1 || m_nambaTrack == 3 || m_nambaTrack == 5 || m_nambaTrack == 7))//��g�����Ԑ�
	{
		m_pilotLampR = false;
		doorYama = 2;
	}
}

inline void CDoorcontrol::DoorCls(void)noexcept//�ԏ����������
{
	m_sanoF = false;
	m_nambaF = false;
	m_pilotLamp = true;
	m_pilotLampL = true;
	m_pilotLampR = true;
	m_OpenTime = 0ms;
	doorUmi = 0;
	doorYama = 0;
}

inline void CDoorcontrol::NambaDoorOpn(void)noexcept//�^�]�m����ԑ��̔����J����
{

if (m_nambaF && !m_pilotLamp && m_pilotLampR && (m_nambaTrack == 2 || m_nambaTrack == 4 || m_nambaTrack == 6 || m_nambaTrack == 8))//��g�����Ԑ�
	{
		m_DoorOpnR->Start();
		doorYama = 2;
		m_pilotLampR = false;
		if (m_nambaTrack == 2 || m_nambaTrack == 4)
		{
			m_joushaOpen = true;
		}
	}
	else if (m_nambaF && !m_pilotLamp && m_pilotLampL && (m_nambaTrack == 1 || m_nambaTrack == 3 || m_nambaTrack == 5 || m_nambaTrack == 7))//��g��Ԑ�
	{
		m_DoorOpnL->Start();
		doorUmi = 2;
		m_pilotLampL = false;
		if (m_nambaTrack == 1)//��g1�Ԑ��̓z�[���h�A���J����
		{
			m_HomeDoorOpn->Start();
		}
		if (m_nambaTrack == 1 || m_nambaTrack == 3)
		{
			m_joushaOpen = true;
		}
	}
}

inline void CDoorcontrol::NambaDoorOpnK(void)noexcept//�^�]�m���~�ԑ��̔����J����i������j
{
		if (m_nambaF && !m_pilotLamp && !m_doorSWOpen && (m_nambaTrack == 2 || m_nambaTrack == 4))//��g�����Ԑ�
		{
			m_DoorClsL.flag = false;
			if (!m_DoorClsL.isRunning())
			{
				if (doorUmi == 1)
				{
					m_DoorOpnL.setPlayLength(nullptr, nullptr, nullptr, nullptr, 0U);
					m_DoorOpnL->Start();
				}
			}
			else
			{
				float rate = static_cast<float>(m_DoorClsL.Position().count()) / static_cast<float>(m_DoorClsL.Duration().count());
				auto startTime = std::chrono::duration_cast<winrt::Windows::Foundation::TimeSpan>(m_DoorOpnL.Duration() * (1.0f - rate));
				m_DoorOpnL.setPlayLength(startTime, m_DoorOpnL.Duration() - startTime, nullptr, nullptr, 0U);
				m_DoorClsL->Stop();
				m_DoorOpnL->Start();
			}
			m_doorSWOpen = true;
			doorUmi = 2;
		}
		else if (m_nambaF && !m_pilotLamp && !m_doorSWOpen && (m_nambaTrack == 1 || m_nambaTrack == 3))//��g��Ԑ�
		{
			m_DoorClsR.flag = false;
			if (!m_DoorClsR.isRunning())
			{
				if (doorYama == 1)
				{
					m_DoorOpnR.setPlayLength(nullptr, nullptr, nullptr, nullptr, 0U);//���S�ɕ��Ă���ꍇ
					m_DoorOpnR->Start();
				}
			}
			else
			{
				float rate = static_cast<float>(m_DoorClsR.Position().count()) / static_cast<float>(m_DoorClsR.Duration().count());
				auto startTime = std::chrono::duration_cast<winrt::Windows::Foundation::TimeSpan>(m_DoorOpnR.Duration() * (1.0f - rate));
				m_DoorOpnR.setPlayLength(startTime, m_DoorOpnR.Duration() - startTime, nullptr, nullptr, 0U);
				m_DoorClsR->Stop();
				m_DoorOpnR->Start();
			}
			m_doorSWOpen = true;
			doorYama = 2;
		}
}

inline void CDoorcontrol::NambaDoorClsK(void)noexcept//�^�]�m���~�ԑ��̔������i������j
{
		if (m_nambaF && !m_pilotLamp && m_doorSWOpen && (m_nambaTrack == 2 || m_nambaTrack == 4))//��g�����Ԑ�
		{
			m_DoorClsL.flag = true;
			if (!m_DoorOpnL.isRunning())m_DoorClsL.setPlayLength(nullptr, nullptr, nullptr, nullptr, 0U);
			else
			{
				float rate = static_cast<float>(m_DoorOpnL.Position().count()) / static_cast<float>(m_DoorOpnL.Duration().count());
				auto startTime = std::chrono::duration_cast<winrt::Windows::Foundation::TimeSpan>(m_DoorClsL.Duration() * (1.0f - rate));
				m_DoorClsL.setPlayLength(startTime, m_DoorClsL.Duration() - startTime, nullptr, nullptr, 0U);
				m_DoorOpnL->Stop();
			}
			m_DoorClsL->Start();
			m_doorSWOpen = false;
		}
		else if (m_nambaF && !m_pilotLamp && m_doorSWOpen && (m_nambaTrack == 1 || m_nambaTrack == 3))//��g��Ԑ�
		{
			m_DoorClsR.flag = true;
			if (!m_DoorOpnR.isRunning())m_DoorClsR.setPlayLength(nullptr, nullptr, nullptr, nullptr, 0U);
			else
			{
				float rate = static_cast<float>(m_DoorOpnR.Position().count()) / static_cast<float>(m_DoorOpnR.Duration().count());
				auto startTime = std::chrono::duration_cast<winrt::Windows::Foundation::TimeSpan>(m_DoorClsR.Duration() * (1.0f - rate));
				m_DoorClsR.setPlayLength(startTime, m_DoorClsR.Duration() - startTime, nullptr, nullptr, 0U);
				m_DoorOpnR->Stop();
			}
			m_DoorClsR->Start();
			m_doorSWOpen = false;
		}
}

#endif //_CDOOR_CONTROL_INCLUDED