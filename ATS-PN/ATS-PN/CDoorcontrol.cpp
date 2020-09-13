#pragma once
#include "pch.h"
#include "CDoorcontrol.h"

CDoorcontrol::CDoorcontrol(std::wstring moduleDir, IXAudio2* pXau2)
{
	m_pXAudio2 = pXau2;
	m_module_dir = moduleDir;
	m_tableFileName = m_module_dir + L"doorConfig.csv";
	init();
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
	table.imbue(std::locale("ja-JP"));
	if (!table.fail())
	{
		while (!table.eof())
		{
			std::string loadline;
			std::getline(table, loadline);
			std::vector<std::string> columun;
			cleanUpBveStr(&loadline);
			if (!loadline.empty())
			{
				splitCsvColumn(loadline, &columun);
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

void CDoorcontrol::Running(const int time)
{
	static int time_pre = 0, deltaT = 0;
	deltaT = time - time_pre;
	if (deltaT < 0 || deltaT >= 1000)deltaT = 0;//âwÇ…à⁄ìÆëŒçÙ
	if (!m_pilotLamp)m_OpenTime += deltaT;


	if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 2 || m_nambaTrack == 4 || m_nambaTrack == 6 || m_nambaTrack == 8))//ìÔîgãÙêîî‘ê¸
	{
		if (m_OpenTime_pre < DOOR_TIMER_NAMBA && m_OpenTime >= DOOR_TIMER_NAMBA)
		{
			if(m_DoorClsL)m_DoorClsL->flag = true;
			if(m_DoorClsL)m_DoorClsL->Start();
		}
		if(m_DoorClsL)if (m_DoorClsL->flag && !m_DoorClsL->isRunning())doorUmi = 1, m_DoorClsL->flag = false, m_pilotLampL = true;
	}
	else if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 1 || m_nambaTrack == 3 || m_nambaTrack == 5 || m_nambaTrack == 7))//ìÔîgäÔêîî‘ê¸
	{
		if (m_OpenTime_pre < DOOR_TIMER_NAMBA && m_OpenTime >= DOOR_TIMER_NAMBA)
		{
			if(m_DoorClsR)m_DoorClsR->flag = true;
			if(m_DoorClsR)m_DoorClsR->Start();
		}
		if(m_DoorClsR)if (m_DoorClsR->flag && !m_DoorClsR->isRunning())doorYama = 1, m_DoorClsR->flag = false, m_pilotLampR = true;
	}
	else if (m_sanoF && !m_pilotLamp && (m_sanoTrack == 2 || m_sanoTrack == 3))//êÚç≤ñÏ2(3)î‘èÊÇËèÍ
	{
		if (m_OpenTime_pre < DOOR_TIMER_OPN && m_OpenTime >= DOOR_TIMER_OPN)
		{
			if(m_DoorOpnR)m_DoorOpnR->Start();
			doorUmi = 2;
			m_pilotLampR = false;
		}
		if (m_OpenTime_pre < DOOR_TIMER_CLS2 && m_OpenTime >= DOOR_TIMER_CLS2)
		{
			if(m_DoorClsR)m_DoorClsR->flag = true;
			if(m_DoorClsR)m_DoorClsR->Start();
		}
		if (m_DoorClsR)if (m_DoorClsR->flag && !m_DoorClsR->isRunning())doorUmi = 0, m_DoorClsR->flag = false, m_pilotLampR = true;
	}
	else if (m_sanoF && !m_pilotLamp && (m_sanoTrack == 5 || m_sanoTrack == 4))//êÚç≤ñÏ5(4)î‘èÊÇËèÍ
	{
		if (m_OpenTime_pre < DOOR_TIMER_OPN && m_OpenTime >= DOOR_TIMER_OPN)
		{
			if(m_DoorOpnR)m_DoorOpnR->Start();
			doorYama = 2;
			m_pilotLampR = false;
		}
		if (m_OpenTime_pre < DOOR_TIMER_CLS5 && m_OpenTime >= DOOR_TIMER_CLS5)
		{
			if(m_DoorClsR)m_DoorClsR->flag = true;
			if(m_DoorClsR)m_DoorClsR->Start();
		}
		if (m_DoorClsR)if (m_DoorClsR->flag && !m_DoorClsR->isRunning())doorYama = 0, m_DoorClsR->flag = false, m_pilotLampR = true;
	}
	else
	{
		doorUmi = 0;
		doorYama = 0;
	}
	m_OpenTime_pre = m_OpenTime;
	time_pre = time;
}

void CDoorcontrol::Halt(int staNo)
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
	if (m_nambaF && !m_pilotLamp && m_pilotLampR && (m_nambaTrack == 2 || m_nambaTrack == 4 || m_nambaTrack == 6 || m_nambaTrack == 8))//ìÔîgãÙêîî‘ê¸
	{
		if (m_DoorOpnR)m_DoorOpnR->Start();
		doorYama = 2;
		m_pilotLampR = false;
	}
	else if (m_nambaF && !m_pilotLamp && m_pilotLampL && (m_nambaTrack == 1 || m_nambaTrack == 3 || m_nambaTrack == 5 || m_nambaTrack == 7))//ìÔîgäÔêîî‘ê¸
	{
		if(m_DoorOpnL)m_DoorOpnL->Start();
		doorUmi = 2;
		m_pilotLampL = false;
	}
}

void CDoorcontrol::init(void)
{
	doorUmi = 0;
	doorYama = 0;
	m_trainNo = 0;
	m_DoorClsL = nullptr;
	m_DoorClsR = nullptr;
	m_DoorOpnL = nullptr;
	m_DoorOpnR = nullptr;
	m_nambaF = false;
	m_nambaTrack = 0;
	m_OpenTime = 0, m_OpenTime_pre = 0;
	m_sanoF = false;
	m_sanoTrack = 0;
	m_trainNo = 0;
	loadconfig();
}

void CDoorcontrol::loadconfig(void)
{
	std::wstring DoorClsL_name;
	std::wstring DoorClsR_name;
	std::wstring DoorOpnL_name;
	std::wstring DoorOpnR_name;
	std::wifstream Config(m_module_dir + L"DoorSoundConfig.txt");
	if (!Config.fail())
	{
		Config.imbue(std::locale("ja-JP"));
		while (!Config.eof())
		{
			std::wstring loadline;
			std::getline(Config, loadline);
			cleanUpBveStr(&loadline);
			size_t l_equal;
			l_equal = loadline.find_first_of('=');
			std::wstring left, right;
			left = loadline.substr(0, l_equal);
			right = loadline.substr(l_equal + 1);


				if (icasecmp(left, static_cast<std::wstring>(L"CloseLeft")))DoorClsL_name = m_module_dir + right;
				else if (icasecmp(left, static_cast<std::wstring>(L"CloseRight")))DoorClsR_name = m_module_dir + right;
				else if (icasecmp(left, static_cast<std::wstring>(L"OpenLeft")))DoorOpnL_name = m_module_dir + right;
				else if (icasecmp(left, static_cast<std::wstring>(L"OpenRight")))DoorOpnR_name = m_module_dir + right;
		}
	}
	Config.close();
	if (m_pXAudio2)
	{
		m_DoorClsL = new CSourceVoice(m_pXAudio2, DoorClsL_name, 1);
		m_DoorClsR = new CSourceVoice(m_pXAudio2, DoorClsR_name, 1);
		m_DoorOpnL = new CSourceVoice(m_pXAudio2, DoorOpnL_name, 1);
		m_DoorOpnR = new CSourceVoice(m_pXAudio2, DoorOpnR_name, 1);
	}
	if (m_DoorClsL)m_DoorClsL->SetVolume(1.0f);
	if (m_DoorClsR)m_DoorClsR->SetVolume(1.0f);
	if (m_DoorOpnL)m_DoorOpnL->SetVolume(1.0f);
	if (m_DoorOpnR)m_DoorOpnR->SetVolume(1.0f);
}
