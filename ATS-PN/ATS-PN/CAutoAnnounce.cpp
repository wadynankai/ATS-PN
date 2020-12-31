#include "CAutoAnnounce.h"

CAutoAnnounce::CAutoAnnounce(const std::filesystem::path& moduleDir, IXAudio2* pXau2) :
	m_trainNo(0),//�����\�ԍ�
	m_staNo(0),//�w�ԍ�
	m_Location(0.0), m_Location_pre(0.0),//������
	//	m_LocationOrigin = 0.0;//�����܂����u�Ԃ̋�����
	m_AnnounceMode(AnnounceMode::Commuter),
	//	m_RunDistance = 0.0, m_RunDistance_pre = 0.0;//�o�����Ă���̋���
	m_set_no(false),//��x�ł�105�Ԓn��q�𓥂񂾂��Ƃ���������true
	m_A_Loc1((std::numeric_limits<double>::max)()),//�o�������̋�������ۑ�
	m_A_Loc2((std::numeric_limits<double>::max)()),//���������̋�������ۑ�
	m_pXAudio2(pXau2),
	m_module_dir(moduleDir),
	m_table_dir(moduleDir / L"announce\\"),
	micGauge(0.0f)
{}

CAutoAnnounce::~CAutoAnnounce() noexcept
{
	m_Announce1 = nullptr;
	m_Announce2 = nullptr;
}

void CAutoAnnounce::setTrainNo(int number)
{
	m_trainNo = number;
	std::wifstream table(m_table_dir / (std::to_wstring(m_trainNo) + L".csv"));
	table.imbue(std::locale("ja-JP"));
//	std::wofstream ofs(m_module_dir + L"anndebug.txt");
	if (!table.fail())
	{
		while (!table.eof())
		{
			std::wstring loadline;
			std::getline(table, loadline);//�s��ǂݍ���
			std::vector<std::wstring> columun;
			cleanUpBveStr(&loadline, table.getloc());//�R�����g��X�y�[�X������
			if (!loadline.empty())//���������Ă��Ȃ����R�����g�����̎��͉������Ȃ��B
			{
				splitCsvColumn(loadline, &columun, table.getloc());//�s���Z�����Ƃɕ�����B

				if (columun.at(0) == L"first")//�n���w
				{
					m_first.sta_no = 0;//�Ȃ�ł�����
					if (columun.size() > 1)if (!columun.at(1).empty())m_first.name1 = m_table_dir / columun.at(1);//���ԃA�i�E���X
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
					if (columun.size() > 3)if (!columun.at(3).empty())m_first.name2 = m_table_dir / columun.at(3);//�����A�i�E���X
					if (columun.size() > 4)
					{
						if (!columun.at(4).empty() && std::all_of(columun.at(4).cbegin(), columun.at(4).cend(), isdigit))m_first.location2 = std::stod(columun.at(4));//�����A�i�E���X������
						else m_first.location2 = std::numeric_limits<double>::max();
					}
//						ofs << std::to_wstring(m_first.sta_no) << L"," << m_first.name1 << L"," << m_first.name2 << L"," << std::to_wstring(m_first.location2) << std::endl;
				}
				else
				{
					AnnounceSet buf;
					if (!columun.at(0).empty() && (std::all_of(columun.at(0).cbegin(), columun.at(0).cend(), isdigit)))buf.sta_no = std::stoi(columun.at(0));//�w�ԍ�
					else buf.sta_no = std::numeric_limits<int>::max();
					if (columun.size() > 1)if (!columun.at(1).empty())buf.name1 = m_table_dir / columun.at(1);//���ԃA�i�E���X		if (columun.size() > 2)
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
					if (columun.size() > 3)if (!columun.at(3).empty())buf.name2 = m_table_dir / columun.at(3);//�����A�i�E���X
					if (columun.size() > 4)
					{
						if (!columun.at(4).empty() && std::all_of(columun.at(4).cbegin(), columun.at(4).cend(), isdigit))buf.location2 = std::stod(columun.at(4));//�����A�i�E���X������
						else buf.location2 = std::numeric_limits<double>::max();
					}
//						ofs << std::to_wstring(buf.sta_no) << L"," << buf.name1 << L"," << buf.name2 << L"," << std::to_wstring(buf.dist2) << std::endl;
					m_A_Set.emplace_back(std::move(buf));
				}
			}
		}
		table.close();
//		ofs.close();
	}
	m_A_Set.shrink_to_fit();
}



