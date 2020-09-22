#pragma once
//#include "pch.h"
#include <vector>
#include <string>
#include "LoadBveText.h"

constexpr auto DEP_DISTANCE_COM = 50.0;//�ʋΎҏo����̃A�i�E���X��50m���s��ɖ炷�B
constexpr auto DEP_DISTANCE_R_FIRST = 80.0;//���s�[�g�n���w�̃A�i�E���X��80m���s��ɖ炷�B
constexpr auto DEP_DISTANCE_S_FIRST = 180.0;//�T�U���v���~�A���n���w�̃A�i�E���X��150m���s��ɖ炷�B
constexpr auto DEP_DISTANCE_LIM = 300.0;//��C�����}�o����̃A�i�E���X��300m���s��ɖ炷�B
constexpr auto DEP_DISTANCE_K = 100.0;//��������}�n���w�̃A�i�E���X��100m���s��ɖ炷�B
enum class AnnounceMode
{
	manual, Commuter, Rapit, Southern, Koya
};
struct AnnounceSet//CSV�t�@�C���̍s��ۑ�
{
	int sta_no = 0;//�w�ԍ�
	std::wstring name1;//�o����̃A�i�E���X
	AnnounceMode mode = AnnounceMode::Commuter;
	double location1 = DBL_MAX;//�����O�̃A�i�E���X�̈ʒu
	std::wstring name2;//�����O�̃A�i�E���X
	double location2 = DBL_MAX;//�����O�̃A�i�E���X�̈ʒu
};
class CAutoAnnounce
{
public:
	CAutoAnnounce(const std::wstring& moduleDir, IXAudio2* pXau2 = nullptr);
	~CAutoAnnounce();
	void setTrainNo(int);
	void Running(const double,const int);
	void Halt(const int);
	void DoorCls(void);

private:
	std::wstring m_module_dir;//�v���O�C���̃f�B���N�g��
	std::wstring m_table_dir;//�ݒ�t�@�C���̃f�B���N�g��
	int m_trainNo = 0;//�����\�ԍ�
	int m_staNo = 0;//�w�ԍ�
	double m_Location = 0.0, m_Location_pre = 0.0;//������
//	double m_LocationOrigin = 0.0;//�����܂����u�Ԃ̋�����
//	double m_RunDistance = 0.0, m_RunDistance_pre = 0.0;//�o�����Ă���̋���
	bool m_set_no = false;//��x�ł�105�Ԓn��q�𓥂񂾂��Ƃ���������true
	AnnounceSet m_first;//�n���w�̐ݒ��ۑ�
	std::vector<AnnounceSet> m_A_Set;//�n���w�ȊO�̐ݒ��ۑ�
	AnnounceMode m_AnnounceMode;
	double m_A_Loc1 = DBL_MAX;//�o�������̋�������ۑ�
	double m_A_Loc2 = DBL_MAX;//���������̋�������ۑ�

	IXAudio2* m_pXAudio2 = nullptr;
	CSourceVoice* m_Announce1 = nullptr, * m_Announce2 = nullptr;


//	std::wofstream ofs;
};

