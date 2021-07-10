// ATS-PN.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//
#include "ATS-PN.h"
// Called when this plug-in is loaded
ATS_API void WINAPI atsLoad(void)
{
//	try
//	{
		if(!CATSPN::GetInstance())CATSPN::CreateInstance(g_module_dir, g_TrainSpeed, g_deltaT, g_deltaL, g_Brake);
/*	}
	catch (const std::exception& exp)
	{
		std::string str("PNパターン読み込み時に例外発生\r\n");
		str += exp.what();
		MessageBoxA(nullptr, str.c_str(), "ATS-PN.dll", MB_OK);
		std::exit(EXIT_FAILURE);
	}*/
	HRESULT hr;
	if (FAILED(hr = XAudio2Create(pXAudio2.put(), 0, XAUDIO2_DEFAULT_PROCESSOR)))//XAudio2初期化失敗
	{
		pXAudio2 = {};
	}
	else
	{
		if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, NULL, NULL, AudioCategory_GameEffects)))
		{
			if (pMasteringVoice)pMasteringVoice->DestroyVoice(), pMasteringVoice = nullptr;//MasteringVoiceを消す
			pXAudio2 = {};//XAudio2をやめる
		}
		else
		{
			g_Ding.reset(pXAudio2, g_module_dir / L"atsDing.wav", XAUDIO2_LOOP_INFINITE, XAUDIO2_VOICE_NOPITCH);
			g_Ding1.reset(pXAudio2, g_module_dir / L"atsDing1.wav", 0, XAUDIO2_VOICE_NOPITCH);
			g_Ding2.reset(pXAudio2, g_module_dir / L"atsDing2.wav", 0, XAUDIO2_VOICE_NOPITCH);
			WAVEFORMATEX wfx = { 0 };
			g_Ding.getFormat(&wfx);
			UINT32 LoopEnd = wfx.nSamplesPerSec * 42 / 1000;
			g_Ding.setPlayLength(0, 0, 0, LoopEnd, XAUDIO2_LOOP_INFINITE);
			g_Ding->Stop();
			g_Ding->SetVolume(1.0);
			g_Ding1->SetVolume(1.0);
			g_Ding2->SetVolume(1.0);
		}
	}
	std::filesystem::path TraponConfig(g_module_dir / "TraponConfig.txt");
	if (std::filesystem::exists(TraponConfig))
	{
		std::ifstream Config;
		Config.open(TraponConfig);
		if (Config.is_open())
		{
			Config.imbue(std::locale("ja-JP"));
			std::string loadline;
			std::getline(Config, loadline);
			cleanUpBveStr(loadline, Config.getloc());
			CTrapon::GetInstance().setTransitionCount(std::stoi(loadline));
		}
		Config.close();
	}
	else
	{
		MessageBoxA(nullptr, (TraponConfig.string() + "が見つかりません").c_str(), "ATS-PN.dll", MB_OK);
	}
	g_Space = false;
	g_delete_push = false;
	g_PgUp_push = false;
	g_PgDn_push = false;
}

// Called when this plug-in is unloaded
ATS_API void WINAPI atsDispose(void)
{
	pXAudio2 = {};//dllの場合，デストラクタに解放処理を任せられない？
}

// Returns the version numbers of ATS plug-in
ATS_API int WINAPI atsGetPluginVersion(void)
{
	return ATS_VERSION;
}

// Called when the train is loaded
ATS_API void WINAPI atsSetVehicleSpec(ATS_VEHICLESPEC vehicleSpec)
{
	g_svcBrake = vehicleSpec.BrakeNotches;//常用最大ブレーキ
	g_emgBrake = vehicleSpec.BrakeNotches + 1;//非常ブレーキ
}

// Called when the game is started
ATS_API void WINAPI atsInitialize(int brake)
{
	if(CATSPN::GetInstance())CATSPN::GetInstance()->initATSPN();
}

// Called every frame
ATS_API ATS_HANDLES WINAPI atsElapse(ATS_VEHICLESTATE vehicleState, int* panel, int* sound)
{
	ATS_HANDLES ret = { 0 };//出力
	static bool l_pause = false;//trueのときに音を止める（ローカル変数）
	if (l_pause && pXAudio2)pXAudio2->StartEngine(), l_pause = false;
	if (GetKeyState('P') & 0x80)l_pause = true;

	g_deltaT = vehicleState.Time - g_time;
	g_time = vehicleState.Time;
	g_deltaL = vehicleState.Location - g_location;
	g_location = vehicleState.Location;
	g_TrainSpeed = vehicleState.Speed;

	if (CATSPN::GetInstance())CATSPN::GetInstance()->RunPNcontrol();//PN制御実行
	g_AstTimer += g_deltaT;
	if (g_AstTimer >= 1000)
	{
		g_Aster = !g_Aster;
		g_AstTimer %= 1000;
	}
	else if (g_AstTimer < 0)g_AstTimer = 0;


	panel[99] = CTrapon::GetInstance().getBackGround(g_deltaT);//トラポン背景出力
	panel[100] = CTrapon::GetInstance().getTimeTable();//時刻表

	if (!CTrapon::GetInstance().getPower())//トラポンの電源が切れているとき
	{
		//表示
		panel[2] = 0;//GPS
		panel[3] = 0;//P接近
		panel[5] = 0;//ブレーキ動作
		panel[6] = 0;//PN制御
		panel[193] = 0;//速度制限
		panel[199] = 0;//駅名点滅
		panel[208] = 0;//駅通防止
		panel[233] = 0;//終端防護
		panel[255] = 0;//アスタリスク
	}
	else [[likely]]//トラポンの電源が入っているとき
	{
		//表示
		panel[2] = true;//GPS
		if (CATSPN::GetInstance())
		{
			panel[3] = CATSPN::GetInstance()->PatternApproachDisp;//P接近
			panel[5] = (CATSPN::GetInstance()->svcBrake || CATSPN::GetInstance()->emgBrake);//ブレーキ動作
			panel[6] = CATSPN::GetInstance()->PNcontrolDisp;//PN制御
			panel[193] = CATSPN::GetInstance()->SpeedLimitDisp;//速度制限
			if (CATSPN::GetInstance()->StationName == 0)//駅名点滅していないとき
			{
				panel[199] = 0;
			}
			else
			{
				if (!CTrapon::GetInstance().getColor())
				{
					panel[199] = CATSPN::GetInstance()->StationName;//駅名点滅（黒画面）
				}
				else
				{
					panel[199] = CATSPN::GetInstance()->StationName + 2;//駅名点滅（白画面）
				}
			}
			panel[208] = CATSPN::GetInstance()->haltDisp;//駅通防止
			panel[233] = CATSPN::GetInstance()->TerminalSafetyDisp;//終端防護
		}
		panel[255] = g_Aster;//アスタリスク
	}



	//音
	if (CATSPN::GetInstance())
	{
		sound[CATSPN::GetInstance()->HaltSound] = CATSPN::GetInstance()->HaltSound();//駅通防止チャイム
		sound[CATSPN::GetInstance()->ApproachSound] = CATSPN::GetInstance()->ApproachSound();//「接近，接近」
	}
	if (CDoorcontrol::GetInstance())
	{
		CDoorcontrol::GetInstance()->Running(vehicleState.Time);
		panel[217] = CDoorcontrol::GetInstance()->doorYama;
		panel[218] = CDoorcontrol::GetInstance()->doorUmi;
	}
	if (CAutoAnnounce::GetInstance())
	{
		CAutoAnnounce::GetInstance()->Running(vehicleState.Location);
		panel[213] = static_cast<int>(CAutoAnnounce::GetInstance()->micGauge * 10.0f);
	}

	ret.Reverser = g_Reverser;
	ret.Power = g_Power;
	ret.Brake = g_Brake;//正常時
	if (CATSPN::GetInstance())
	{
		if (CATSPN::GetInstance()->emgBrake)ret.Brake = g_emgBrake;//非常指令
		else if (CATSPN::GetInstance()->svcBrake && ret.Brake != g_emgBrake)ret.Brake = g_svcBrake;//常用最大指令
	}
	ret.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;

	if (g_ShasyouBell)
	{
		if (g_Bell1)
		{
			g_belltimer += g_deltaT;
		}
		if (g_belltimer > 1500)
		{
			if (g_Ding1.flag)
			{
				g_Ding1->Start();
				g_Ding1.flag = false;
			}
			else if (g_Ding2.flag)
			{
				g_Ding2->Start();
				g_Ding2.flag = false;
			}
		}
	}


	if (l_pause)//ポーズのとき
	{
		if (pXAudio2)pXAudio2->StopEngine();
	}

	return ret;
}

// Called when the power is changed
ATS_API void WINAPI atsSetPower(int notch)
{
	g_Power = notch;
}

// Called when the brake is changed
ATS_API void WINAPI atsSetBrake(int notch)
{
	g_Brake = notch;
}

// Called when the reverser is changed
ATS_API void WINAPI atsSetReverser(int pos)
{
	g_Reverser = pos;
}

// Called when any ATS key is pressed
ATS_API void WINAPI atsKeyDown(int atsKeyCode)
{
	switch (atsKeyCode)
	{
	case ATS_KEY_S:
		if (!g_Space)
		{
			g_Ding->Stop();
			g_Ding.reSubmitSourceBuffer();
			g_Ding->Start();
			g_Space = true;
			g_belltimer = 0;
		}
		break;
	case ATS_KEY_A2:
		if (!g_delete_push)
		{
			if (GetKeyState(VK_SHIFT) & 0x80)
			{
				CTrapon::GetInstance().IcCard();
			}
			else [[likely]]
			{
				CTrapon::GetInstance().changeColor();
			}
			g_delete_push = true;
		}
		break;
	case ATS_KEY_C1:
		if (!g_PgUp_push) 
		{
			if (GetKeyState(VK_SHIFT) & 0x80)
			{
				CTrapon::GetInstance().nextTimeTable();
			}
			g_PgUp_push = true;
		}
		break;
	case ATS_KEY_C2:
		if (!g_PgDn_push)
		{
			if (GetKeyState(VK_SHIFT) & 0x80)
			{
				CTrapon::GetInstance().prevTimeTable();
			}
			g_PgDn_push = true;
		}
		break;
	}

}

// Called when any ATS key is released
ATS_API void WINAPI atsKeyUp(int atsKeyCode)
{
	switch (atsKeyCode)
	{
	case ATS_KEY_S:
		if (g_Space)
		{
			g_Ding->ExitLoop();
			g_Space = false;
			if (g_ShasyouBell)
			{
				if (!g_Bell1)
				{
					g_Bell1 = true;
					g_Ding1.flag = true;
					g_belltimer = 0;
				}
				else if (!g_Bell2)
				{
					g_Bell2 = true;
					g_Ding1.flag = false;
					g_Ding2.flag = true;
					g_belltimer = 0;
				}
			}
		}
		break;
	case ATS_KEY_B1:
		if (CATSPN::GetInstance())CATSPN::GetInstance()->resetATSPN();
		break;
	case ATS_KEY_A1:
		if (GetKeyState(VK_SHIFT) & 0x80)
		{
			CTrapon::GetInstance().powerButton();
		}
		else [[likely]]
		{
			if (CDoorcontrol::GetInstance())CDoorcontrol::GetInstance()->NambaDoorOpn();
		}
		break;
	case ATS_KEY_A2:
		if (g_delete_push)g_delete_push = false;
		break;
	case ATS_KEY_C1:
		if (g_PgUp_push)g_PgUp_push = false;
		break;
	case ATS_KEY_C2:
		if (g_PgDn_push)g_PgDn_push = false;
		break;
	}


}

// Called when the horn is used
ATS_API void WINAPI atsHornBlow(int hornType)
{

}

// Called when the door is opened
ATS_API void WINAPI atsDoorOpen(void)
{
	if (CATSPN::GetInstance())CATSPN::GetInstance()->haltOFF();
	if (CDoorcontrol::GetInstance())
	{
		CDoorcontrol::GetInstance()->DoorOpn();
	}
	g_ShasyouBell = false;
	g_Bell1 = false;
	g_Bell2 = false;
}

// Called when the door is closed
ATS_API void WINAPI atsDoorClose(void)
{
	if (CATSPN::GetInstance())CATSPN::GetInstance()->haltOFF();
	if (CDoorcontrol::GetInstance())
	{
		CDoorcontrol::GetInstance()->DoorCls();
	}
	if (CAutoAnnounce::GetInstance())
	{
		CAutoAnnounce::GetInstance()->DoorCls();
	}
/*	if ((g_sta_no.at(0) == 2 && g_sta_no.at(1) == 3)
		|| (g_sta_no.at(0) == 3 && g_sta_no.at(1) == 5)
		|| (g_sta_no.at(0) == 31 && g_sta_no.at(1) == 30)
		|| (g_sta_no.at(0) == 44 && g_sta_no.at(1) == 43)
		|| (g_sta_no.at(0) == 76 && g_sta_no.at(1) == 75)
		|| (g_sta_no.at(0) == 78 && g_sta_no.at(1) == 79)
		|| (g_sta_no.at(0) == 85 && g_sta_no.at(1) == 84)
		|| (g_sta_no.at(0) == 105 && g_sta_no.at(1) == 104))
	{
		g_ShasyouBell = true;
	}*/
}

// Called when current signal is changed
ATS_API void WINAPI atsSetSignal(int signal)
{

}

// Called when the beacon data is received
ATS_API void WINAPI atsSetBeaconData(ATS_BEACONDATA beaconData)
{
	switch (beaconData.Type)
	{
	case PN_Beacon::SpdLim:
		if (CATSPN::GetInstance())CATSPN::GetInstance()->LimitSpeedON(beaconData.Optional);
		break;
	case PN_Beacon::StopPatern:
		if (CATSPN::GetInstance())CATSPN::GetInstance()->stopPattern(beaconData.Optional);
		break;
	case PN_Beacon::Defeat:
		if (CATSPN::GetInstance())CATSPN::GetInstance()->LimitSpeedOFF();
		break;
	case PN_Beacon::Terminal:
		if (CATSPN::GetInstance())CATSPN::GetInstance()->TerminalSafetyON(beaconData.Optional);
		break;
	case PN_Beacon::Halt:
//		g_sta_no.at(1) = g_sta_no.at(0);
//		g_sta_no.at(0) = beaconData.Optional;
		if (beaconData.Optional != 1
			&& beaconData.Optional != 32
			&& beaconData.Optional != 45
			&& beaconData.Optional != 46
			&& beaconData.Optional != 77
			&& beaconData.Optional != 86
			&& beaconData.Optional != 106)
		{
			if (CATSPN::GetInstance())CATSPN::GetInstance()->haltON(beaconData.Optional);
		}
		else g_ShasyouBell = true;
		if (CDoorcontrol::GetInstance())
		{
			CDoorcontrol::GetInstance()->Halt(beaconData.Optional);
		}
		if (CAutoAnnounce::GetInstance())
		{
			CAutoAnnounce::GetInstance()->Halt(beaconData.Optional);
		}
		break;
	case PN_Beacon::LineLim:
		if (CATSPN::GetInstance())CATSPN::GetInstance()->LineMax(beaconData.Optional);
		break;
	case PN_Beacon::Timetable:
		g_timetable = beaconData.Optional % 1000;
		CTrapon::GetInstance().addTrainNo(g_timetable);
		CTrapon::GetInstance().wakeUp();
		if (CDoorcontrol::GetInstance())
		{
			CDoorcontrol::GetInstance()->setTrainNo(g_timetable);
		}
		else
		{
			CDoorcontrol::CreateInstance(g_module_dir, pXAudio2);
			if (CDoorcontrol::GetInstance())
			{
				CDoorcontrol::GetInstance()->setTrainNo(g_timetable);
			}
		}
		if (beaconData.Optional >= 1000)
		{
			if (CAutoAnnounce::GetInstance())
			{
				CAutoAnnounce::GetInstance()->setTrainNo(beaconData.Optional);
			}
			else
			{
				CAutoAnnounce::CreateInstance(g_module_dir, pXAudio2, g_deltaT);
				if (CAutoAnnounce::GetInstance())
				{
					CAutoAnnounce::GetInstance()->setTrainNo(beaconData.Optional);
				}
			}
		}
		break;
	}
}





