#pragma once
#include "pch.h"
#include "CAutoAnnounce.h"

CAutoAnnounce::CAutoAnnounce(const std::wstring& moduleDir, IXAudio2* pXau2) :
	m_trainNo(0),//�����\�ԍ�
	m_staNo(0),//�w�ԍ�
	m_Location(0.0), m_Location_pre(0.0),//������
	//	m_LocationOrigin = 0.0;//�����܂����u�Ԃ̋�����
	m_AnnounceMode(AnnounceMode::Commuter),
	//	m_RunDistance = 0.0, m_RunDistance_pre = 0.0;//�o�����Ă���̋���
	m_set_no(false),//��x�ł�105�Ԓn��q�𓥂񂾂��Ƃ���������true
	m_A_Loc1(DBL_MAX),//�o�������̋�������ۑ�
	m_A_Loc2(DBL_MAX),//���������̋�������ۑ�
	m_pXAudio2(pXau2),
	m_module_dir(moduleDir),
	m_table_dir(moduleDir + L"announce\\") 
{}

CAutoAnnounce::~CAutoAnnounce()
{
	SAFE_DELETE(m_Announce1);
	SAFE_DELETE(m_Announce2);
}

void CAutoAnnounce::setTrainNo(int number)
{
	m_trainNo = number;
	std::wifstream table(m_table_dir + std::to_wstring(m_trainNo) + L".csv");
	table.imbue(std::locale("ja-JP"));
//	std::wofstream ofs(m_module_dir + L"anndebug.txt");
	if (!table.fail())
	{
		while (!table.eof())
		{
			std::wstring loadline;
			std::getline(table, loadline);//�s��ǂݍ���
			std::vector<std::wstring> columun;
			cleanUpBveStr(&loadline);//�R�����g��X�y�[�X������
			if (!loadline.empty())//���������Ă��Ȃ����R�����g�����̎��͉������Ȃ��B
			{
				splitCsvColumn(loadline, &columun);//�s���Z�����Ƃɕ�����B

				if (columun.at(0) == L"first")//�n���w
				{
					m_first.sta_no = 0;//�Ȃ�ł�����
					if (columun.size() > 1)if (!columun.at(1).empty())m_first.name1 = m_table_dir + columun.at(1);//���ԃA�i�E���X
					if (columun.size() > 2)
					{
						if (columun.at(2) == L"COM")m_first.mode = AnnounceMode::Commuter;
						else if (columun.at(2) == L"R")m_first.mode = AnnounceMode::Rapit;
						else if (columun.at(2) == L"SP")m_first.mode = AnnounceMode::Southern;
						else if (columun.at(2) == L"K")m_first.mode = AnnounceMode::Koya;
						else if (!columun.at(2).empty() && std::all_of(columun.at(2).cbegin(), columun.at(2).cend(), isdigit))
						{
							m_first.mode = AnnounceMode::manual;
							m_first.location1 = std::stod(columun.at(2));
						}
						else m_first.mode = AnnounceMode::Commuter;
					}
					if (columun.size() > 3)if (!columun.at(3).empty())m_first.name2 = m_table_dir + columun.at(3);//�����A�i�E���X
					if (columun.size() > 4)
					{
						if (!columun.at(4).empty() && std::all_of(columun.at(4).cbegin(), columun.at(4).cend(), isdigit))m_first.location2 = std::stod(columun.at(4));//�����A�i�E���X������
						else m_first.location2 = DBL_MAX;
					}
					m_first.name1.shrink_to_fit();
					m_first.name2.shrink_to_fit();
//						ofs << std::to_wstring(m_first.sta_no) << L"," << m_first.name1 << L"," << m_first.name2 << L"," << std::to_wstring(m_first.dist2) << std::endl;
				}
				else
				{
					AnnounceSet buf;
					if (!columun.at(0).empty() && (std::all_of(columun.at(0).cbegin(), columun.at(0).cend(), isdigit)))buf.sta_no = std::stoi(columun.at(0));//�w�ԍ�
					else buf.sta_no = INT_MAX;
					if (columun.size() > 1)if (!columun.at(1).empty())buf.name1 = m_table_dir + columun.at(1);//���ԃA�i�E���X		if (columun.size() > 2)
					{
						if (columun.at(2) == L"COM")buf.mode = AnnounceMode::Commuter;
						else if (columun.at(2) == L"R")buf.mode = AnnounceMode::Rapit;
						else if (columun.at(2) == L"SP")buf.mode = AnnounceMode::Southern;
						else if (columun.at(2) == L"K")buf.mode = AnnounceMode::Koya;
						else if (!columun.at(2).empty() && std::all_of(columun.at(2).cbegin(), columun.at(2).cend(), isdigit))
						{
							buf.mode = AnnounceMode::manual;
							buf.location1 = std::stod(columun.at(2));
						}
						else buf.mode = AnnounceMode::Commuter;
					}
					if (columun.size() > 3)if (!columun.at(3).empty())buf.name2 = m_table_dir + columun.at(3);//�����A�i�E���X
					if (columun.size() > 4)
					{
						if (!columun.at(4).empty() && std::all_of(columun.at(4).cbegin(), columun.at(4).cend(), isdigit))buf.location2 = std::stod(columun.at(4));//�����A�i�E���X������
						else buf.location2 = DBL_MAX;
					}
					buf.name1.shrink_to_fit();
					buf.name2.shrink_to_fit();
//						ofs << std::to_wstring(buf.sta_no) << L"," << buf.name1 << L"," << buf.name2 << L"," << std::to_wstring(buf.dist2) << std::endl;
					m_A_Set.emplace_back(buf);
				}
			}
		}
		table.close();
//		ofs.close();
	}
	m_A_Set.shrink_to_fit();
}

void CAutoAnnounce::Running(const double loc, const int time)
{
	static int time_pre = 0;//�O�t���[���̎���
	int delT = 0;//1�t���[���̎���
	delT = time - time_pre;
	m_Location = loc;//���݂̈ʒu
//	m_RunDistance = m_Location - m_LocationOrigin;//�o�����Ă���̋���

	if (delT >= 1000 || delT < 0)//�w�Ɉړ������Ƃ�
	{
		if (m_Announce1)m_Announce1->Stop();
		if (m_Announce2)m_Announce2->Stop();
		if (m_Location >= m_A_Loc1)SAFE_DELETE(m_Announce1);
		if (m_Location >= m_A_Loc2)SAFE_DELETE(m_Announce2);
	}
	if (m_Announce1)if (m_Location_pre < m_A_Loc1 && m_Location >= m_A_Loc1)m_Announce1->Start();
	if (m_Announce2)if (m_Location_pre < m_A_Loc2 && m_Location >= m_A_Loc2)m_Announce2->Start();


	m_Location_pre = m_Location;
//	m_RunDistance_pre = m_RunDistance;
	time_pre = time;
}

void CAutoAnnounce::Halt(const int no)
{
	if(!m_set_no)m_set_no = true;
	m_staNo = no;
}

void CAutoAnnounce::DoorCls(void)
{
//	m_LocationOrigin = m_Location;//�w���Ԏ��̈ʒu��o�^�i�o����̕����Ɏg�p�j
	SAFE_DELETE(m_Announce1);//�O�̕���������
	SAFE_DELETE(m_Announce2);//�O�̕���������
	if (m_set_no)//�n���w�ȊO
	{
		for (const auto& a : m_A_Set)
		{
			if (a.sta_no == m_staNo)
			{
				HRESULT hr;
				BOOL mfStarted = FALSE;//���f�B�A�t�@���f�[�V�����v���b�g�t�H�[����������������TRUE�ɂ���B
				hr = MFStartup(MF_VERSION);// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[���̏�����
				mfStarted = SUCCEEDED(hr);//�������o������TRUE�ɂ���B
				if (!a.name1.empty())m_Announce1 = new CSourceVoice(m_pXAudio2, a.name1, 0);//������o�^
				if (!a.name2.empty())m_Announce2 = new CSourceVoice(m_pXAudio2, a.name2, 0);//������o�^
				if (mfStarted)MFShutdown();// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[��������������Ă�����I��
				switch (a.mode)//�o��������̈ʒu��o�^
				{
				case AnnounceMode::Commuter:
					m_A_Loc1 = m_Location + DEP_DISTANCE_COM;
					break;
				case AnnounceMode::Rapit:
				case AnnounceMode::Southern:
					m_A_Loc1 = m_Location + DEP_DISTANCE_LIM;
					break;
				case AnnounceMode::Koya:
					m_A_Loc1 = m_Location + DEP_DISTANCE_K;
					break;
				case AnnounceMode::manual:
					m_A_Loc1 = a.location1;
					break;
				}
				m_A_Loc2 = a.location2;//�����O�����̈ʒu��o�^
				if (m_Announce1)m_Announce1->SetVolume(1.0f);//���ʂ�1�ɐݒ肷��i���ꂪ�Ȃ��ƁC����0�ɂȂ�j
				if (m_Announce2)m_Announce2->SetVolume(1.0f);//���ʂ�1�ɐݒ肷��i���ꂪ�Ȃ��ƁC����0�ɂȂ�j
				break;
			}
		}
	}
	else//�n���w
	{
		HRESULT hr;
		BOOL mfStarted = FALSE;//���f�B�A�t�@���f�[�V�����v���b�g�t�H�[����������������TRUE�ɂ���B
		hr = MFStartup(MF_VERSION);// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[���̏�����
		mfStarted = SUCCEEDED(hr);//�������o������TRUE�ɂ���B
		if (!m_first.name1.empty())m_Announce1 = new CSourceVoice(m_pXAudio2, m_first.name1, 0);//������o�^
		if (!m_first.name2.empty())m_Announce2 = new CSourceVoice(m_pXAudio2, m_first.name2, 0);//������o�^
		if (mfStarted)MFShutdown();// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[��������������Ă�����I��
		switch (m_first.mode)//�o��������̈ʒu��o�^
		{
		case AnnounceMode::Commuter:
			m_A_Loc1 = m_Location + DEP_DISTANCE_COM;
			break;
		case AnnounceMode::Rapit:
			m_A_Loc1 = m_Location + DEP_DISTANCE_R_FIRST;
			break;
		case AnnounceMode::Southern:
			m_A_Loc1 = m_Location + DEP_DISTANCE_S_FIRST;
			break;
		case AnnounceMode::Koya:
			m_A_Loc1 = m_Location + DEP_DISTANCE_K;
			break;
		case AnnounceMode::manual:
			m_A_Loc1 = m_first.location1;
			break;
		}
		m_A_Loc2 = m_first.location2;//�����O�����̈ʒu��o�^
		if (m_Announce1)m_Announce1->SetVolume(1.0f);//���ʂ�1�ɐݒ肷��i���ꂪ�Ȃ��ƁC����0�ɂȂ�j
		if (m_Announce2)m_Announce2->SetVolume(1.0f);//���ʂ�1�ɐݒ肷��i���ꂪ�Ȃ��ƁC����0�ɂȂ�j
	}
}

