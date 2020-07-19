#pragma once
#include "stdafx.h"
#include "atsplugin.h"
#include "CSourceVoice.h"
#include <vector>
#include <string>
#include <fstream>

#define DOOR_TIMER_OPN 7000//泉佐野で右側の扉が開くまでの時間[ms]
#define DOOR_TIMER_CLS2 35000//泉佐野で2(3)番のりば到着時右側の扉が閉まるまでの時間[ms]
#define DOOR_TIMER_CLS5 20000//泉佐野で5(4)番のりば到着時右側の扉が閉まるまでの時間[ms]
#define DOOR_TIMER_NAMBA 20000//難波で乗車側の扉が開いてから降車側の扉が閉じるまでの時間[ms]

class CDoorcontrol
{
public:
	CDoorcontrol();
	CDoorcontrol(std::wstring, int, IXAudio2* pXau2 = nullptr);
	~CDoorcontrol();
	void setTrainNo(int);
	void Running(ATS_VEHICLESTATE&);
	void NextSano(void);
	void NextNamba(void);
	void DoorOpn(void);
	void DoorCls(void);
	void NambaDoorOpn(void);
	int doorYama = 0;//ドア山インジケータ―(0:透明,1:消灯,2:点灯)
	int doorUmi = 0;//ドア海インジケータ―(0:透明,1:消灯,2:点灯)
	int DoorClsL = ATS_SOUND_CONTINUE, DoorClsR = ATS_SOUND_CONTINUE, DoorOpnL = ATS_SOUND_CONTINUE, DoorOpnR = ATS_SOUND_CONTINUE;//ドア開閉音

private:
	void init(void);
	void loadconfig(void);
	bool m_pilotLampL = true, m_pilotLampR = true;
	std::wstring m_module_dir;
	std::wstring m_tableFileName;
	bool m_pilotLamp;//BVE本体のドア「閉」時にtrue
	int m_trainNo = 0;
	bool m_sanoF, m_nambaF;//次の駅が泉佐野や難波であることを示すフラグ
	int m_sanoOpnTimer = 0, m_sanoClsTimer = 0, m_nambaOpnTimer2 = 0;
	int m_OpenTime = 0, m_OpenTime_pre = 0;//ドアが開いてからの時間
	int m_sanoTrack = 0, m_nambaTrack = 0;//泉佐野と難波の番線

	IXAudio2* m_pXAudio2 = nullptr;
	CSourceVoice* m_DoorClsL = nullptr, *m_DoorClsR = nullptr, *m_DoorOpnL = nullptr, *m_DoorOpnR = nullptr;

	//コメントやスペースを消去する
	void cleanUpBveStr(std::string&);
	void cleanUpBveStr(std::wstring&);
	//csvの行を分割する。
	void splitCsvColumn(std::string&, std::vector<std::string>&);
	void splitCsvColumn(std::wstring&, std::vector<std::wstring>&);
	//大文字と小文字を区別しない文字比較(等しいときにtrue)
	bool icasecmp(const std::string& l, const std::string& r);
	bool icasecmp(const std::wstring& l, const std::wstring& r);

};

