#include "CDoorcontrol.h"

inline void CDoorcontrol::loadconfig(void)
{
	std::filesystem::path DoorClsL_name;
	std::filesystem::path DoorClsR_name;
	std::filesystem::path DoorOpnL_name;
	std::filesystem::path DoorOpnR_name;
	std::wifstream Config(m_module_dir / L"DoorSoundConfig.txt");
	if (!Config.fail())
	{
		Config.imbue(std::locale("ja-JP"));
		while (!Config.eof())
		{
			std::wstring loadline;
			std::getline(Config, loadline);
			cleanUpBveStr(&loadline, Config.getloc());
			std::wstring left, right;
			splitSymbol(L'=', loadline, &left, &right, Config.getloc());
			if (icasecmp(left, L"Close Left", Config.getloc()))DoorClsL_name = m_module_dir / right;
			else if (icasecmp(left, L"Close Right", Config.getloc()))DoorClsR_name = m_module_dir / right;
			else if (icasecmp(left, L"Open Left", Config.getloc()))DoorOpnL_name = m_module_dir / right;
			else if (icasecmp(left, L"Open Right", Config.getloc()))DoorOpnR_name = m_module_dir / right;
		}
	}
	Config.close();
	if (m_pXAudio2)
	{
		m_DoorClsL = { m_pXAudio2, DoorClsL_name, 0, XAUDIO2_VOICE_NOPITCH };
		m_DoorClsR = { m_pXAudio2, DoorClsR_name, 0, XAUDIO2_VOICE_NOPITCH };
		m_DoorOpnL = { m_pXAudio2, DoorOpnL_name, 0, XAUDIO2_VOICE_NOPITCH };
		m_DoorOpnR = { m_pXAudio2, DoorOpnR_name, 0, XAUDIO2_VOICE_NOPITCH };
	}
	if (m_DoorClsL)m_DoorClsL->SetVolume(1.0f);
	if (m_DoorClsR)m_DoorClsR->SetVolume(1.0f);
	if (m_DoorOpnL)m_DoorOpnL->SetVolume(1.0f);
	if (m_DoorOpnR)m_DoorOpnR->SetVolume(1.0f);
}

CDoorcontrol::CDoorcontrol(const std::filesystem::path& moduleDir, IXAudio2* pXau2) :
	m_pXAudio2(pXau2),
	m_module_dir(moduleDir),
	m_tableFileName(m_module_dir / L"doorConfig.csv"),
	doorUmi(0), doorYama(0), m_trainNo(0),
	m_DoorClsL(nullptr), m_DoorClsR(nullptr), m_DoorOpnL(nullptr), m_DoorOpnR(nullptr),
	m_nambaF(false), m_nambaTrack(0),
	m_OpenTime(0), m_OpenTime_pre(0),
	m_sanoF(false), m_sanoTrack(0)
{
	loadconfig();
}

CDoorcontrol::~CDoorcontrol() noexcept
{
}

void CDoorcontrol::setTrainNo(const int no)
{
	m_trainNo = no;
	std::ifstream table(m_tableFileName);
	table.imbue(std::locale("ja-JP"));
	if (!table.fail())
	{
		while (!table.eof())
		{
			std::string loadline;
			std::getline(table, loadline);
			std::vector<std::string> columun;
			cleanUpBveStr(&loadline, table.getloc());
			if (!loadline.empty())
			{
				splitCsvColumn(loadline, &columun, table.getloc());
				if (std::stoi(columun.at(0)) == m_trainNo&& columun.size()>=3)
				{
					if (!columun.at(1).empty() && (std::all_of(columun.at(1).cbegin(), columun.at(1).cend(), isdigit)))m_nambaTrack = std::stoi(columun.at(1));
					if (!columun.at(2).empty() && (std::all_of(columun.at(2).cbegin(), columun.at(2).cend(), isdigit)))m_sanoTrack = std::stoi(columun.at(2));
				}
			}
		}
	}
	table.close();
}




