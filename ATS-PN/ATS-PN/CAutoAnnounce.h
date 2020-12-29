#ifndef _CAUTO_ANNOUNCE_INCLUDED_
#define _CAUTO_ANNOUNCE_INCLUDED_
#define NOMINMAX
#include <fstream>
#include <locale>
#include <vector>
#include <filesystem>
#include <algorithm>
#include "..\..\common\LoadBveText.h"
#include "..\..\common\CSourceVoice.h"

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
	CAutoAnnounce(const std::filesystem::path& moduleDir, IXAudio2* pXau2 = nullptr);
	~CAutoAnnounce();
	void setTrainNo(int);
	inline void Running(const double,const int);
	inline void Halt(const int);
	inline void DoorCls(void);
	float micGauge;

private:
	std::filesystem::path m_module_dir;//�v���O�C���̃f�B���N�g��
	std::filesystem::path m_table_dir;//�ݒ�t�@�C���̃f�B���N�g��
	int m_trainNo = 0;//�����\�ԍ�
	int m_staNo = 0;//�w�ԍ�
	double m_Location = 0.0, m_Location_pre = 0.0;//������
//	double m_LocationOrigin = 0.0;//�����܂����u�Ԃ̋�����
//	double m_RunDistance = 0.0, m_RunDistance_pre = 0.0;//�o�����Ă���̋���
	bool m_set_no = false;//��x�ł�105�Ԓn��q�𓥂񂾂��Ƃ���������true
	bool m_pilotLamp = false;//�h�A���܂��Ă�����true
	AnnounceSet m_first;//�n���w�̐ݒ��ۑ�
	std::vector<AnnounceSet> m_A_Set;//�n���w�ȊO�̐ݒ��ۑ�
	AnnounceMode m_AnnounceMode;
	double m_A_Loc1 = std::numeric_limits<double>::max();//�o�������̋�������ۑ�
	double m_A_Loc2 = std::numeric_limits<double>::max();//���������̋�������ۑ�

	IXAudio2* m_pXAudio2 = nullptr;
	CSourceVoice m_Announce1, m_Announce2;


//	std::wofstream ofs;
};



inline void CAutoAnnounce::Running(const double loc, const int time)
{
	static int time_pre = 0;//�O�t���[���̎���
	int delT = 0;//1�t���[���̎���
	delT = time - time_pre;
	m_Location = loc;//���݂̈ʒu
//	m_RunDistance = m_Location - m_LocationOrigin;//�o�����Ă���̋���

	if (m_Location_pre < m_A_Loc1 && m_Location >= m_A_Loc1)m_Announce1->Start();
	if (m_Location_pre < m_A_Loc2 && m_Location >= m_A_Loc2)m_Announce2->Start();
	if (delT >= 1000 || delT < 0)//�w�Ɉړ������Ƃ�
	{
		if (m_Announce1)m_Announce1->Stop();
		if (m_Announce2)m_Announce2->Stop();
	}

	micGauge = std::min(m_Announce1.getLevel() + m_Announce2.getLevel(), 1.0f);

	m_Location_pre = m_Location;
	//	m_RunDistance_pre = m_RunDistance;
	time_pre = time;
}


inline void CAutoAnnounce::Halt(const int no)
{
	if (!m_set_no)m_set_no = true;
	m_staNo = no;
}

inline void CAutoAnnounce::DoorCls(void)
{
	//	m_LocationOrigin = m_Location;//�w���Ԏ��̈ʒu��o�^�i�o����̕����Ɏg�p�j
	if (m_set_no)//�n���w�ȊO
	{
		for (const auto& a : m_A_Set)
		{
			if (a.sta_no == m_staNo)
			{
				HRESULT hr;
				bool mfStarted = false;//���f�B�A�t�@���f�[�V�����v���b�g�t�H�[����������������TRUE�ɂ���B
				hr = MFStartup(MF_VERSION);// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[���̏�����
				mfStarted = SUCCEEDED(hr);//�������o������TRUE�ɂ���B
				if (!a.name1.empty())
				{
					m_Announce1.reset(m_pXAudio2, a.name1, 0, XAUDIO2_VOICE_NOPITCH);//������o�^
				}
				else m_Announce1.reset(nullptr);//�O�̕���������
				if (!a.name2.empty())
				{
					m_Announce2.reset(m_pXAudio2, a.name2, 0, XAUDIO2_VOICE_NOPITCH);//������o�^
				}
				else m_Announce2.reset();//�O�̕���������
				if (mfStarted)MFShutdown();// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[��������������Ă�����I��
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
				m_Announce1->SetVolume(1.0f);//���ʂ�1�ɐݒ肷��i���ꂪ�Ȃ��ƁC����0�ɂȂ�j
				m_Announce2->SetVolume(1.0f);//���ʂ�1�ɐݒ肷��i���ꂪ�Ȃ��ƁC����0�ɂȂ�j
				break;
			}
		}
	}
	else//�n���w
	{
		HRESULT hr;
		bool mfStarted = false;//���f�B�A�t�@���f�[�V�����v���b�g�t�H�[����������������TRUE�ɂ���B
		hr = MFStartup(MF_VERSION);// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[���̏�����
		mfStarted = SUCCEEDED(hr);//�������o������TRUE�ɂ���B
		if (!m_first.name1.empty())m_Announce1.reset(m_pXAudio2, m_first.name1, 0, XAUDIO2_VOICE_NOPITCH);//������o�^
		if (!m_first.name2.empty())m_Announce2.reset(m_pXAudio2, m_first.name2, 0, XAUDIO2_VOICE_NOPITCH);//������o�^
		if (mfStarted)MFShutdown();// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[��������������Ă�����I��
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
		m_Announce1->SetVolume(1.0f);//���ʂ�1�ɐݒ肷��i���ꂪ�Ȃ��ƁC����0�ɂȂ�j
		m_Announce2->SetVolume(1.0f);//���ʂ�1�ɐݒ肷��i���ꂪ�Ȃ��ƁC����0�ɂȂ�j
	}
}


#endif // !_CAUTO_ANNOUNCE_INCLUDED_
