#ifndef _CDOOR_CONTROL_INCLUDED
#define _CDOOR_CONTROL_INCLUDED

#include <filesystem>
#include <fstream>
#include <thread>
#include "..\..\common\LoadBveText.h"
#include "..\..\common\CAudioFileInputNode.h"

#ifdef EXCEPTION
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
#endif
#include <windows.h>
#include <source_location>
#endif // EXCEPTION

namespace DoorTimer
{
	constexpr int open = 7000;//泉佐野で右側の扉が開くまでの時間[ms]
	constexpr int SanoCls2 = 35000;//泉佐野で2(3)番のりば到着時右側の扉が閉まるまでの時間[ms]
	constexpr int SanoCls5 = 20000;//泉佐野で5(4)番のりば到着時右側の扉が閉まるまでの時間[ms]
	constexpr int NambaCls = 20000;//難波で乗車側の扉が開いてから降車側の扉が閉じるまでの時間[ms]
};

class CDoorcontrol
{
public:
	~CDoorcontrol();
	static void CreateInstance(const std::filesystem::path& moduleDir,
		const winrt::Windows::Media::Audio::AudioGraph& graph = nullptr, const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode = nullptr)
	{
		if (!instance)instance.reset(new CDoorcontrol(moduleDir, graph, outputNode));
	}
/*	static void Delete()noexcept
	{
		instance.reset();
	}*/
	static std::unique_ptr<CDoorcontrol>& GetInstance()noexcept
	{
		return instance;
	}
	void setTrainNo(const int);
	inline void Running(const int)noexcept;
	inline void Halt(const int)noexcept;
	inline void DoorOpn(void)noexcept;
	inline void DoorCls(void)noexcept;
	inline void NambaDoorOpn(void)noexcept;
	int doorYama = 0;//ドア山インジケータ―(0:透明,1:消灯,2:点灯)
	int doorUmi = 0;//ドア海インジケータ―(0:透明,1:消灯,2:点灯)

private:
	CDoorcontrol() = delete;
	CDoorcontrol(const std::filesystem::path& moduleDir, 
		const winrt::Windows::Media::Audio::AudioGraph& graph = nullptr, const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode = nullptr);
	CDoorcontrol& operator=(CDoorcontrol&) = delete;
	CDoorcontrol& operator=(CDoorcontrol&&) = delete;
	inline static std::unique_ptr<CDoorcontrol> instance;
	void loadconfig(void);
	bool m_pilotLampL = true, m_pilotLampR = true;
	std::filesystem::path m_module_dir;
	std::filesystem::path m_tableFileName;
	bool m_pilotLamp;//BVE本体のドア「閉」時にtrue
	int m_trainNo = 0;
	bool m_sanoF, m_nambaF;//次の駅が泉佐野や難波であることを示すフラグ
	int m_OpenTime = 0, m_OpenTime_pre = 0;//ドアが開いてからの時間
	int m_sanoTrack = 0, m_nambaTrack = 0;//泉佐野と難波の番線

	winrt::Windows::Media::Audio::AudioGraph m_graph;
	winrt::Windows::Media::Audio::AudioDeviceOutputNode m_outputNode;
	bool m_graphCreated = false, m_outputNodeCreated = false;
	CAudioFileInputNode m_DoorClsL = nullptr, m_DoorClsR = nullptr, m_DoorOpnL = nullptr, m_DoorOpnR = nullptr;
};


inline void CDoorcontrol::Running(const int time) noexcept
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	static int time_pre = 0, deltaT = 0;
	deltaT = time - time_pre;
	if (deltaT < 0 || deltaT >= 1000)deltaT = 0;//駅に移動対策
	if (!m_pilotLamp)m_OpenTime += deltaT;


	if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 2 || m_nambaTrack == 4 || m_nambaTrack == 6 || m_nambaTrack == 8))//難波偶数番線
	{
		if (m_OpenTime_pre < DoorTimer::NambaCls && m_OpenTime >= DoorTimer::NambaCls)
		{
			if (m_DoorClsL)m_DoorClsL.flag = true;
			if (m_DoorClsL)m_DoorClsL.Start();
		}
		if (m_DoorClsL)if (m_DoorClsL.flag && !m_DoorClsL.isRunning())doorUmi = 1, m_DoorClsL.flag = false, m_pilotLampL = true;
	}
	else if (m_nambaF && !m_pilotLamp && (m_nambaTrack == 1 || m_nambaTrack == 3 || m_nambaTrack == 5 || m_nambaTrack == 7))//難波奇数番線
	{
		if (m_OpenTime_pre < DoorTimer::NambaCls && m_OpenTime >= DoorTimer::NambaCls)
		{
			if (m_DoorClsR)m_DoorClsR.flag = true;
			if (m_DoorClsR)m_DoorClsR.Start();
		}
		if (m_DoorClsR)if (m_DoorClsR.flag && !m_DoorClsR.isRunning())doorYama = 1, m_DoorClsR.flag = false, m_pilotLampR = true;
	}
	else if (m_sanoF && !m_pilotLamp && (m_sanoTrack == 2 || m_sanoTrack == 3))//泉佐野2(3)番乗り場
	{
		if (m_OpenTime_pre < DoorTimer::open && m_OpenTime >= DoorTimer::open)
		{
			if (m_DoorOpnR)m_DoorOpnR.Start();
			doorUmi = 2;
			m_pilotLampR = false;
		}
		if (m_OpenTime_pre < DoorTimer::SanoCls2 && m_OpenTime >= DoorTimer::SanoCls2)
		{
			if (m_DoorClsR)m_DoorClsR.flag = true;
			if (m_DoorClsR)m_DoorClsR.Start();
		}
		if (m_DoorClsR)if (m_DoorClsR.flag && !m_DoorClsR.isRunning())doorUmi = 0, m_DoorClsR.flag = false, m_pilotLampR = true;
	}
	else if (m_sanoF && !m_pilotLamp && (m_sanoTrack == 5 || m_sanoTrack == 4))//泉佐野5(4)番乗り場
	{
		if (m_OpenTime_pre < DoorTimer::open && m_OpenTime >= DoorTimer::open)
		{
			if (m_DoorOpnR)m_DoorOpnR.Start();
			doorYama = 2;
			m_pilotLampR = false;
		}
		if (m_OpenTime_pre < DoorTimer::SanoCls5 && m_OpenTime >= DoorTimer::SanoCls5)
		{
			if (m_DoorClsR)m_DoorClsR.flag = true;
			if (m_DoorClsR)m_DoorClsR.Start();
		}
		if (m_DoorClsR)if (m_DoorClsR.flag && !m_DoorClsR.isRunning())doorYama = 0, m_DoorClsR.flag = false, m_pilotLampR = true;
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
		ss1 << L"エラーコード：" << std::hex << hr.code() << L"\n" << hr.message().c_str();
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

inline void CDoorcontrol::DoorOpn(void)noexcept
{
	m_pilotLamp = false;
	m_OpenTime = 0;
}

inline void CDoorcontrol::DoorCls(void)noexcept
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

inline void CDoorcontrol::NambaDoorOpn(void)noexcept
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	if (m_nambaF && !m_pilotLamp && m_pilotLampR && (m_nambaTrack == 2 || m_nambaTrack == 4 || m_nambaTrack == 6 || m_nambaTrack == 8))//難波偶数番線
	{
		if (m_DoorOpnR)m_DoorOpnR.Start();
		doorYama = 2;
		m_pilotLampR = false;
	}
	else if (m_nambaF && !m_pilotLamp && m_pilotLampL && (m_nambaTrack == 1 || m_nambaTrack == 3 || m_nambaTrack == 5 || m_nambaTrack == 7))//難波奇数番線
	{
		if (m_DoorOpnL)m_DoorOpnL.Start();
		doorUmi = 2;
		m_pilotLampL = false;
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
		ss1 << L"エラーコード：" << std::hex << hr.code() << L"\n" << hr.message().c_str();
		ss2 << std::source_location::current().function_name();
		MessageBox(nullptr, ss1.str().c_str(), ss2.str().c_str(), MB_OK);
	}
#endif // EXCEPTION
}


#endif //_CDOOR_CONTROL_INCLUDED