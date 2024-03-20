#ifndef _CAUTO_ANNOUNCE_INCLUDED_
#define _CAUTO_ANNOUNCE_INCLUDED_
#define NOMINMAX

#include <limits>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
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

namespace dep_distance
{
	constexpr double commuter = 50.0;//�ʋΎҏo����̃A�i�E���X��50m���s��ɖ炷�B
	constexpr double rapi_t_first = 80.0;//���s�[�g�n���w�̃A�i�E���X��80m���s��ɖ炷�B
	constexpr double southern_first = 180.0;//�T�U���v���~�A���n���w�̃A�i�E���X��150m���s��ɖ炷�B
	constexpr double honsen_ltd_exp = 300.0;//��C�����}�o����̃A�i�E���X��300m���s��ɖ炷�B
	constexpr double koya_ltd_exp = 100.0;//��������}�n���w�̃A�i�E���X��100m���s��ɖ炷�B
};

enum class AnnounceMode
{
	manual, Commuter, Rapit, Southern, Koya
};
struct AnnounceSet//CSV�t�@�C���̍s��ۑ�
{
	int sta_no = 0;//�w�ԍ�
	std::filesystem::path name1;//�o����̃A�i�E���X
	AnnounceMode mode = AnnounceMode::Commuter;
	double location1 = std::numeric_limits<double>::max();//�����O�̃A�i�E���X�̈ʒu
	std::filesystem::path name2;//�����O�̃A�i�E���X
	double location2 = std::numeric_limits<double>::max();//�����O�̃A�i�E���X�̈ʒu
};
class CAutoAnnounce
{
public:
	~CAutoAnnounce()noexcept;
	static void CreateInstance(const std::filesystem::path& moduleDir, const winrt::com_ptr<IXAudio2>& pXau2, std::chrono::milliseconds& DelT)
	{
		if (!instance)instance.reset(new CAutoAnnounce(moduleDir, pXau2, DelT));
	}
/*	static void Delete()noexcept
	{
		instance.reset();
	}*/
	static std::unique_ptr<CAutoAnnounce>& GetInstance()noexcept
	{
		return instance;
	}
	void setTrainNo(int number);
	inline void Running(const double& loc);
	inline void Halt(const int no) noexcept;
	inline void DoorCls(void) noexcept;
	float micGauge = 0.0f;

private:
	CAutoAnnounce() = delete;
	CAutoAnnounce(const std::filesystem::path& moduleDir, const winrt::com_ptr<IXAudio2>& pXau2, std::chrono::milliseconds& DelT);
	CAutoAnnounce& operator=(CAutoAnnounce&) = delete;
	CAutoAnnounce& operator=(CAutoAnnounce&&) = delete;
	inline static std::unique_ptr<CAutoAnnounce> instance;
	std::filesystem::path m_module_dir;//�v���O�C���̃f�B���N�g��
	std::filesystem::path m_table_dir;//�ݒ�t�@�C���̃f�B���N�g��
	int m_trainNo = 0;//�����\�ԍ�
	int m_staNo = 0;//�w�ԍ�
	std::chrono::milliseconds& m_DelT;//1�t���[���̎���
	double m_Location = 0.0, m_Location_pre = 0.0;//������
//	double m_LocationOrigin = 0.0;//�����܂����u�Ԃ̋�����
//	double m_RunDistance = 0.0, m_RunDistance_pre = 0.0;//�o�����Ă���̋���
	bool m_set_no = false;//��x�ł�105�Ԓn��q�𓥂񂾂��Ƃ���������true
	bool m_pilotLamp = false;//�h�A���܂��Ă�����true
	AnnounceSet m_first{};//�n���w�̐ݒ��ۑ�
	std::vector<AnnounceSet> m_A_Set;//�n���w�ȊO�̐ݒ��ۑ�
	AnnounceMode m_AnnounceMode{ AnnounceMode::Commuter };
	double m_A_Loc1{ std::numeric_limits<double>::max() };//�o�������̋�������ۑ�
	double m_A_Loc2{ std::numeric_limits<double>::max() };//���������̋�������ۑ�

	winrt::com_ptr<IXAudio2> m_pXAudio2;
	CSourceVoice m_Announce1{}, m_Announce2{};
	std::filesystem::path* m_pAnnounce1 = nullptr, * m_pAnnounce2 = nullptr;
	std::thread m_thread1{}, m_thread2{};
	bool m_first_time = true;

//	std::wofstream ofs;
};



inline void CAutoAnnounce::Running(const double& loc)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	static int time_pre = 0;//�O�t���[���̎���
	m_Location = loc;//���݂̈ʒu
//	m_RunDistance = m_Location - m_LocationOrigin;//�o�����Ă���̋���
	if (!m_first_time)
	{
		if (m_Announce1.flag && m_pAnnounce1 && !m_thread2.joinable())//Annouce1��2�𓯎��ɓǂݍ���ł��܂��ƃ��f�B�A�t�@���f�[�V�����̉e���Ńt���[�Y����B
		{
			if (m_thread1.joinable())
			{
				m_thread1.join();
			}
//			Announce1Load = true;//�����������S�Ȃ̂ŃX���b�h����O�ɍs���B
			m_thread1 = std::thread([&] {
				m_Announce1.reset(m_pXAudio2, *m_pAnnounce1, 0, XAUDIO2_VOICE_NOPITCH);
//				Announce1Load = false;
				});//������o�^
			m_Announce1.flag = false;
		}
		if (m_Announce2.flag && m_pAnnounce2 && !m_thread1.joinable())//Annouce1��2�𓯎��ɓǂݍ���ł��܂��ƃ��f�B�A�t�@���f�[�V�����̉e���Ńt���[�Y����B
		{
			if (m_thread2.joinable())
			{
				m_thread2.join();
			}
//			Announce2Load = true;//�����������S�Ȃ̂ŃX���b�h����O�ɍs���B
			m_thread2 = std::thread([&] {
				m_Announce2.reset(m_pXAudio2, *m_pAnnounce2, 0, XAUDIO2_VOICE_NOPITCH);
//				Announce2Load = false;
				});//������o�^
			m_Announce2.flag = false;
		}

		if (m_DelT >= 1000ms || m_DelT <= 0ms)//�w�Ɉړ������Ƃ�
		{
			if (m_thread1.joinable())
			{
				m_thread1.join();
			}
			if (m_thread2.joinable())
			{
				m_thread2.join();
			}
			m_Announce1->Stop();
			m_Announce2->Stop();
		}
		else
		{
			if (m_Location_pre < m_A_Loc1 && m_Location >= m_A_Loc1)
			{
				if (m_thread1.joinable())
				{
					m_thread1.join();
				}
				if (m_Announce2.isRunning())m_Announce2->Stop();
				m_Announce1->Start();
			}
			if (m_Location_pre < m_A_Loc2 && m_Location >= m_A_Loc2)
			{
				if (m_thread2.joinable())
				{
					m_thread2.join();
				}
				if (m_Announce1.isRunning())m_Announce1->Stop();
				m_Announce2->Start();
			}
		}
	}
	else
	{
		m_first_time = false;
	}

	//micGauge = std::min(m_Announce1.getLevel() + m_Announce2.getLevel(), 1.0f);

	m_Location_pre = m_Location;
	//	m_RunDistance_pre = m_RunDistance;
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


inline void CAutoAnnounce::Halt(const int no) noexcept
{
	if (!m_set_no)m_set_no = true;
	m_staNo = no;
}

inline void CAutoAnnounce::DoorCls(void) noexcept
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
		//	m_LocationOrigin = m_Location;//�w���Ԏ��̈ʒu��o�^�i�o����̕����Ɏg�p�j
		if (m_set_no)//�n���w�ȊO
		{
			for (const auto& a : m_A_Set)
			{
				if (a.sta_no == m_staNo)
				{
					if (!a.name1.empty())
					{
						m_Announce1.flag = true;
						m_pAnnounce1 = const_cast<std::filesystem::path*>(&a.name1);//���w�����t�@�C������o�^
					}
					else
					{
						m_Announce1 = nullptr;//�O�̕���������
					}
					if (!a.name2.empty())
					{
						m_Announce2.flag = true;
						m_pAnnounce2 = const_cast<std::filesystem::path*>(&a.name2);//���������t�@�C������o�^
					}
					else
					{
						m_Announce2 = nullptr;//�O�̕���������
					}
					switch (a.mode)//�o��������̈ʒu��o�^
					{
					case AnnounceMode::Commuter:
						m_A_Loc1 = m_Location + dep_distance::commuter;
						break;
					case AnnounceMode::Rapit:
					case AnnounceMode::Southern:
						m_A_Loc1 = m_Location + dep_distance::honsen_ltd_exp;
						break;
					case AnnounceMode::Koya:
						m_A_Loc1 = m_Location + dep_distance::koya_ltd_exp;
						break;
					case AnnounceMode::manual:
						m_A_Loc1 = a.location1;
						break;
					}
					m_A_Loc2 = a.location2;//�����O�����̈ʒu��o�^
					break;
				}
			}
		}
		else//�n���w
		{
			if (!m_first.name1.empty())
			{
				m_Announce1.flag = true;
				m_pAnnounce1 = &m_first.name1;//���w�����t�@�C������o�^
			}
			if (!m_first.name2.empty())
			{
				m_Announce2.flag = true;
				m_pAnnounce2 = &m_first.name2;//���������t�@�C������o�^
			}
			switch (m_first.mode)//�o��������̈ʒu��o�^
			{
			case AnnounceMode::Commuter:
				m_A_Loc1 = m_Location + dep_distance::commuter;
				break;
			case AnnounceMode::Rapit:
				m_A_Loc1 = m_Location + dep_distance::rapi_t_first;
				break;
			case AnnounceMode::Southern:
				m_A_Loc1 = m_Location + dep_distance::southern_first;
				break;
			case AnnounceMode::Koya:
				m_A_Loc1 = m_Location + dep_distance::koya_ltd_exp;
				break;
			case AnnounceMode::manual:
				m_A_Loc1 = m_first.location1;
				break;
			}
			m_A_Loc2 = m_first.location2;//�����O�����̈ʒu��o�^
		}
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
};


#endif // !_CAUTO_ANNOUNCE_INCLUDED_
