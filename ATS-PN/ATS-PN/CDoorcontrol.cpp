#include "CDoorcontrol.h"

void CDoorcontrol::loadconfig(void)
{
	std::filesystem::path DoorClsL_name;
	std::filesystem::path DoorClsR_name;
	std::filesystem::path DoorOpnL_name;
	std::filesystem::path DoorOpnR_name;
	std::filesystem::path HomeDoorOpn_name;
	std::wifstream Config(m_module_dir / L"DoorSoundConfig.txt");
	if (!Config.fail())
	{
		Config.imbue(std::locale(".UTF-8"));
		while (!Config.eof())
		{
			std::wstring loadline;
			std::getline(Config, loadline);
			cleanUpBveStr(loadline, Config.getloc());
			std::wstring left, right;
			if (splitSymbol(L'=', loadline, left, right, Config.getloc()) != std::wstring::npos)
			{
				if (icasecmp(left, L"Close Left", Config.getloc()))DoorClsL_name = m_module_dir / right;
				else if (icasecmp(left, L"Close Right", Config.getloc()))DoorClsR_name = m_module_dir / right;
				else if (icasecmp(left, L"Open Left", Config.getloc()))DoorOpnL_name = m_module_dir / right;
				else if (icasecmp(left, L"Open Right", Config.getloc()))DoorOpnR_name = m_module_dir / right;
				else if (icasecmp(left, L"Home Door", Config.getloc()))HomeDoorOpn_name = m_module_dir / right;
			}
		}
	}
	Config.close();

	if (m_pXAudio2)
	{
		m_DoorClsL.reset(m_pXAudio2, DoorClsL_name, 0, XAUDIO2_VOICE_NOPITCH);
		m_DoorClsR.reset(m_pXAudio2, DoorClsR_name, 0, XAUDIO2_VOICE_NOPITCH);
		m_DoorOpnL.reset(m_pXAudio2, DoorOpnL_name, 0, XAUDIO2_VOICE_NOPITCH);
		m_DoorOpnR.reset(m_pXAudio2, DoorOpnR_name, 0, XAUDIO2_VOICE_NOPITCH);
		m_HomeDoorOpn.reset(m_pXAudio2, HomeDoorOpn_name, 0, XAUDIO2_VOICE_NOPITCH);
	}
	m_DoorClsL->SetVolume(1.0f);
	m_DoorClsR->SetVolume(1.0f);
	m_DoorOpnL->SetVolume(1.0f);
	m_DoorOpnR->SetVolume(1.0f);
	m_HomeDoorOpn->SetVolume(1.0f);
}

CDoorcontrol::CDoorcontrol(const std::filesystem::path& moduleDir, const winrt::com_ptr<IXAudio2>& pXau2) :
	m_pXAudio2(pXau2),
	m_module_dir(moduleDir),
	m_tableFileName(m_module_dir / L"doorConfig.csv"),
	doorUmi(0), doorYama(0), m_trainNo(0),
	m_DoorClsL(nullptr), m_DoorClsR(nullptr), m_DoorOpnL(nullptr), m_DoorOpnR(nullptr),m_HomeDoorOpn(nullptr),
	m_nambaF(false), m_nambaTrack(0),
	m_OpenTime(0), m_OpenTime_pre(0),
	m_sanoF(false), m_sanoTrack(0),
	m_shashouBell{ false,false }
{
	loadconfig();
}

CDoorcontrol::~CDoorcontrol()
{
	m_DoorClsL = nullptr;
	m_DoorClsR = nullptr;
	m_DoorOpnL = nullptr;
	m_DoorOpnR = nullptr;
	m_HomeDoorOpn = nullptr;
}

void CDoorcontrol::setTrainNo(const int no)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	m_trainNo = no;
	std::wifstream table(m_tableFileName);
	table.imbue(std::locale(".UTF-8"));
	if (!table.fail())
	{
		while (!table.eof())
		{
			std::wstring loadline;
			std::getline(table, loadline);
			std::vector<std::wstring> columun;
			cleanUpBveStr(loadline, table.getloc());
			eraseSpace(loadline, table.getloc());
			if (!loadline.empty())
			{
				splitCsvColumn(loadline, columun, table.getloc());
				if (std::stoi(columun.at(0)) == m_trainNo&& columun.size()>=3)
				{
					if (!columun.at(1).empty() && (std::all_of(columun.at(1).cbegin(), columun.at(1).cend(), isdigit)))m_nambaTrack = std::stoi(columun.at(1));
					if (!columun.at(2).empty() && (std::all_of(columun.at(2).cbegin(), columun.at(2).cend(), isdigit)))m_sanoTrack = std::stoi(columun.at(2));
				}
			}
		}
	}
	table.close();
#ifdef EXCEPTION
	}
	catch (std::exception& ex)
	{
		MessageBoxA(nullptr, ex.what(), std::source_location::current().function_name(), MB_OK);
	}
	catch (winrt::hresult_error& hr)
	{
		std::wstringstream ss1, ss2;
		ss1 << L"エラーコード：" << std::hex << hr.code() << L"\n" << hr.message().c_str();
		ss2 << std::source_location::current().function_name();
		MessageBox(nullptr, ss1.str().c_str(), ss2.str().c_str(), MB_OK);
	}
#endif // EXCEPTION
}




