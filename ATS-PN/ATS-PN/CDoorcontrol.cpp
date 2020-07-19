#include "stdafx.h"
#include "CDoorcontrol.h"

CDoorcontrol::CDoorcontrol()
{
	init();
}

CDoorcontrol::CDoorcontrol(std::wstring moduleDir, int TrainNumber,IXAudio2* pXau2)
{
	m_pXAudio2 = pXau2;
	m_module_dir = moduleDir;
	init();
	m_tableFileName = m_module_dir + L"doorConfig.csv";
	setTrainNo(TrainNumber);
}

CDoorcontrol::~CDoorcontrol()
{
	SAFE_DELETE(m_DoorClsL);
	SAFE_DELETE(m_DoorClsR);
	SAFE_DELETE(m_DoorOpnL);
	SAFE_DELETE(m_DoorOpnR);
}

void CDoorcontrol::setTrainNo(int no)
{
	m_trainNo = no;
	std::ifstream table(m_tableFileName);
	if (!table.fail())
	{
		while (!table.eof())
		{
			std::string loadline;
			std::getline(table, loadline);
			std::vector<std::string> columun;
			cleanUpBveStr(loadline);
			if (!loadline.empty())
			{
				splitCsvColumn(loadline, columun);
				if (std::stoi(columun.at(0)) == m_trainNo&& columun.size()==3)
				{
					m_nambaTrack = std::stoi(columun.at(1));
					m_sanoTrack = std::stoi(columun.at(2));
				}
			}
		}
	}


}

void CDoorcontrol::Running(ATS_VEHICLESTATE& vehicleState)
{
	static int time_pre = 0, deltaT = 0;
	deltaT = vehicleState.Time - time_pre;
	if (!m_pilotLamp)m_OpenTime += deltaT;


	if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 2 || m_nambaTrack == 4 || m_nambaTrack == 6 || m_nambaTrack == 8))//ìÔîgãÙêîî‘ê¸
	{
		if (m_OpenTime_pre < DOOR_TIMER_NAMBA && m_OpenTime >= DOOR_TIMER_NAMBA)
		{
			m_DoorClsR->flag = true;
			m_DoorClsR->Start();
		}
		if (m_DoorClsR->flag && !m_DoorClsR->isRunning())doorUmi = 1, m_DoorClsR->flag = false, m_pilotLampR = true;
	}
	else if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 1 || m_nambaTrack == 3 || m_nambaTrack == 5 || m_nambaTrack == 7))//ìÔîgäÔêîî‘ê¸
	{
		if (m_OpenTime_pre < DOOR_TIMER_NAMBA && m_OpenTime >= DOOR_TIMER_NAMBA)
		{
			m_DoorClsL->flag = true;
			m_DoorClsL->Start();
		}
		if (m_DoorClsL->flag && !m_DoorClsL->isRunning())doorYama = 1, m_DoorClsL->flag = false, m_pilotLampL = true;
	}
	else if (m_sanoF && !m_pilotLamp && (m_sanoTrack == 2 || m_sanoTrack == 3))//êÚç≤ñÏ2(3)î‘èÊÇËèÍ
	{
		if (m_OpenTime_pre < DOOR_TIMER_OPN && m_OpenTime >= DOOR_TIMER_OPN)
		{
			m_DoorOpnR->Start();
			doorUmi = 2;
			m_pilotLampR = false;
		}
		if (m_OpenTime_pre < DOOR_TIMER_CLS2 && m_OpenTime >= DOOR_TIMER_CLS2)
		{
			m_DoorClsR->flag = true;
			m_DoorClsR->Start();
		}
		if (m_DoorClsR->flag && !m_DoorClsR->isRunning())doorUmi = 0, m_DoorClsR->flag = false, m_pilotLampR = true;
	}
	else if (m_sanoF && !m_pilotLamp && (m_sanoTrack == 5 || m_sanoTrack == 4))//êÚç≤ñÏ5(4)î‘èÊÇËèÍ
	{
		if (m_OpenTime_pre < DOOR_TIMER_OPN && m_OpenTime >= DOOR_TIMER_OPN)
		{
			m_DoorOpnR->Start();
			doorYama = 2;
			m_pilotLampR = false;
		}
		if (m_OpenTime_pre < DOOR_TIMER_CLS5 && m_OpenTime >= DOOR_TIMER_CLS5)
		{
			m_DoorClsR->flag = true;
			m_DoorClsR->Start();
		}
		if (m_DoorClsR->flag && !m_DoorClsR->isRunning())doorYama = 0, m_DoorClsR->flag = false, m_pilotLampR = true;
	}
	else
	{
		doorUmi = 0;
		doorYama = 0;
	}
	m_OpenTime_pre = m_OpenTime;
	time_pre = vehicleState.Time;
}

void CDoorcontrol::NextSano(void)
{
	m_sanoF = true;
}

void CDoorcontrol::NextNamba(void)
{
	m_nambaF = true;
}

void CDoorcontrol::DoorOpn(void)
{
	m_pilotLamp = false;
	m_OpenTime = 0;
}

void CDoorcontrol::DoorCls(void)
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

void CDoorcontrol::NambaDoorOpn(void)
{
	if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 2 || m_nambaTrack == 4 || m_nambaTrack == 6 || m_nambaTrack == 8))//ìÔîgãÙêîî‘ê¸
	{
		m_DoorOpnL->Start();
		doorYama = 2;
		m_pilotLampL = false;
	}
	else if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 1 || m_nambaTrack == 3 || m_nambaTrack == 5 || m_nambaTrack == 7))//ìÔîgäÔêîî‘ê¸
	{
			m_DoorOpnR->Start();
			doorUmi = 2;
			m_pilotLampR = false;
	}
}

void CDoorcontrol::init(void)
{
	doorUmi = 0;
	doorYama = 0;
	DoorClsL = ATS_SOUND_CONTINUE;
	DoorClsR = ATS_SOUND_CONTINUE;
	DoorOpnL = ATS_SOUND_CONTINUE;
	DoorOpnR = ATS_SOUND_CONTINUE;
	m_trainNo = 0;
	loadconfig();
}

void CDoorcontrol::loadconfig(void)
{
	std::wstring DoorClsL_name;
	std::wstring DoorClsR_name;
	std::wstring DoorOpnL_name;
	std::wstring DoorOpnR_name;
	std::wifstream Config(m_module_dir+L"DoorSoundConfig.txt");
	if (!Config.fail())
	{
		while (!Config.eof())
		{
			std::wstring loadline;
			std::getline(Config, loadline);
			cleanUpBveStr(loadline);
			size_t l_equal;
			l_equal = loadline.find_first_of('=');
			std::wstring left, right;
			left = loadline.substr(0, l_equal);
			right = loadline.substr(l_equal + 1);


				if (icasecmp(left, L"Close Left"))DoorClsL_name = right;
				else if (icasecmp(left, L"Close Right"))DoorClsR_name = right;
				else if (icasecmp(left, L"Open Left"))DoorOpnL_name = right;
				else if (icasecmp(left, L"Open Right"))DoorOpnR_name = right;
		}
	}
	m_DoorClsL = new CSourceVoice(m_pXAudio2, DoorClsL_name, 1);
	m_DoorClsR = new CSourceVoice(m_pXAudio2, DoorClsR_name, 1);
	m_DoorOpnL = new CSourceVoice(m_pXAudio2, DoorOpnL_name, 1);
	m_DoorOpnR = new CSourceVoice(m_pXAudio2, DoorOpnR_name, 1);
	if (m_DoorClsL)m_DoorClsL->SetVolume(1.0f);
	if (m_DoorClsR)m_DoorClsR->SetVolume(1.0f);
	if (m_DoorOpnL)m_DoorOpnL->SetVolume(1.0f);
	if (m_DoorOpnR)m_DoorOpnR->SetVolume(1.0f);
}

void CDoorcontrol::cleanUpBveStr(std::string& StrIn)
{
	size_t comment;
	comment = StrIn.find_first_of(";#");
	if (comment != std::string::npos)StrIn.erase(StrIn.cbegin() + comment, StrIn.cend());
	for (size_t i = 0; i < StrIn.length(); i++)
	{
		if (StrIn.at(i) == (' '))StrIn.erase(StrIn.cbegin() + i);
	}
}

void CDoorcontrol::cleanUpBveStr(std::wstring& StrIn)
{
	size_t comment;
	comment = StrIn.find_first_of(L";#");
	if (comment != std::wstring::npos)StrIn.erase(StrIn.cbegin() + comment, StrIn.cend());
	for (size_t i = 0; i < StrIn.length(); i++)
	{
		if (StrIn.at(i) == (L' '))StrIn.erase(StrIn.cbegin() + i);
	}

}

void CDoorcontrol::splitCsvColumn(std::string& loadline, std::vector<std::string>& columun)
{
	size_t begin = 0U, comma = 0U;
	while (comma < loadline.length())
	{
		if (loadline.at(comma) == ',')
		{
			columun.push_back(loadline.substr(begin, comma - begin));
			begin = comma + 1;
		}
		comma++;
	}
	if (begin < loadline.length()) columun.push_back(loadline.substr(begin));
}

void CDoorcontrol::splitCsvColumn(std::wstring& loadline, std::vector<std::wstring>& columun)
{
	size_t begin = 0U, comma = 0U;
	while (comma < loadline.length())
	{
		if (loadline.at(comma) == ',')
		{
			columun.push_back(loadline.substr(begin, comma - begin));
			begin = comma + 1;
		}
		comma++;
	}
	if (begin < loadline.length()) columun.push_back(loadline.substr(begin));
}

bool CDoorcontrol::icasecmp(const std::string& l, const std::string& r)
{
	return l.size() == r.size()
		&& equal(l.cbegin(), l.cend(), r.cbegin(),
			[](std::string::value_type l1, std::string::value_type r1)
	{ return toupper(l1) == toupper(r1); });
}
bool CDoorcontrol::icasecmp(const std::wstring& l, const std::wstring& r)
{
	return l.size() == r.size()
		&& equal(l.cbegin(), l.cend(), r.cbegin(),
			[](std::wstring::value_type l1, std::wstring::value_type r1)
	{ return towupper(l1) == towupper(r1); });
}