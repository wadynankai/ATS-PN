﻿// ATS-PN.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//
#include "ATS-PN.h"
// Called when this plug-in is loaded
ATS_API void WINAPI atsLoad(void)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION

		if (!CATSPN::GetInstance())CATSPN::CreateInstance(g_module_dir, g_TrainSpeed, g_deltaT, g_deltaL, g_Brake);

	//https://github.com/stenobot/TinyTimer/blob/master/TinyTimer/DataModel/SoundPlayer.cs
		std::thread th{ [&]()
		{
			try
			{
				winrt::Windows::Media::Audio::AudioGraphSettings settings{ winrt::Windows::Media::Render::AudioRenderCategory::Media };
				settings.DesiredRenderDeviceAudioProcessing(winrt::Windows::Media::AudioProcessing::Raw);
				settings.QuantumSizeSelectionMode(winrt::Windows::Media::Audio::QuantumSizeSelectionMode::LowestLatency);
				winrt::Windows::Media::Audio::CreateAudioGraphResult result = winrt::Windows::Media::Audio::AudioGraph::CreateAsync(settings).get();
				if (result.Status() == winrt::Windows::Media::Audio::AudioGraphCreationStatus::Success)
				{
					g_graph = result.Graph();

					winrt::Windows::Media::Audio::CreateAudioDeviceOutputNodeResult oResult = g_graph.CreateDeviceOutputNodeAsync().get();
					if (oResult.Status() == winrt::Windows::Media::Audio::AudioDeviceNodeCreationStatus::Success)
					{
						g_outputNode = oResult.DeviceOutputNode();
					}


					g_Ding = CAudioFileInputNode(g_graph, g_outputNode, g_module_dir / L"atsDing.wav", 0);
					g_Ding0 = CAudioFileInputNode(g_graph, g_outputNode, g_module_dir / L"atsDing0.wav", 0);
					g_Ding1 = CAudioFileInputNode(g_graph, g_outputNode, g_module_dir / L"atsDing1.wav", 0);
					g_Ding2 = CAudioFileInputNode(g_graph, g_outputNode, g_module_dir / L"atsDing2.wav", 0);
					g_graph.Start();
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
#else
			}
			catch(...)
			{ }
#endif //!EXCEPTION
		} 
		};


	std::filesystem::path TraponConfig(g_module_dir / "TraponConfig.txt");
	if (std::filesystem::exists(TraponConfig))
	{
		std::ifstream Config;
		Config.open(TraponConfig);
		if (Config.is_open())
		{
			Config.imbue(std::locale(".UTF-8"));
			std::string loadline;
			std::getline(Config, loadline);
			cleanUpBveStr(loadline, Config.getloc());
			CTrapon::GetInstance().setTransitionCount(std::stoi(loadline));
		}
		Config.close();
	}
	else
	{
		CTrapon::GetInstance().setTransitionCount(4);
	}
	g_Space = false;
	g_delete_push = false;
	g_PgUp_push = false;
	g_PgDn_push = false;

	if(th.joinable()) th.join();
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

// Called when this plug-in is unloaded
ATS_API void WINAPI atsDispose(void)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	std::thread th1{ []() 
		{
			CDoorcontrol::GetInstance().reset();//dllの場合，デストラクタに解放処理を任せられない？
		}
	};
	std::thread th2{ []()
		{
			CAutoAnnounce::GetInstance().reset();//dllの場合，デストラクタに解放処理を任せられない？
		}
	};
	std::thread th{ [&]()
	{
		g_graph.Stop();
		g_Ding = nullptr;
		g_Ding0 = nullptr;
		g_Ding1 = nullptr;
		g_Ding2 = nullptr;
		g_outputNode = nullptr;
		g_graph = nullptr;
	} };
	if (th.joinable())th.join();
	if (th1.joinable())th1.join();
	if (th2.joinable())th2.join();
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

// Returns the version numbers of ATS plug-in
ATS_API int WINAPI atsGetPluginVersion(void)
{
	return ATS_VERSION;
}

// Called when the train is loaded
ATS_API void WINAPI atsSetVehicleSpec(ATS_VEHICLESPEC vehicleSpec)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
//	CATSPN::GetInstance()->setFormationLength(vehicleSpec.Cars);
	g_svcBrake = vehicleSpec.BrakeNotches;//常用最大ブレーキ
	g_emgBrake = vehicleSpec.BrakeNotches + 1;//非常ブレーキ
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

// Called when the game is started
ATS_API void WINAPI atsInitialize(int brake)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	if(CATSPN::GetInstance())CATSPN::GetInstance()->initATSPN();
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

// Called every frame
ATS_API ATS_HANDLES WINAPI atsElapse(ATS_VEHICLESTATE vehicleState, int* panel, int* sound)
{
	ATS_HANDLES ret = { 0 };//出力
#ifdef EXCEPTION
	try {
#endif // EXCEPTION


		static bool l_pause = false;//trueのときに音を止める（ローカル変数）
		if (l_pause && g_graph)g_graph.Start(), l_pause = false;
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
		for (auto& a : CTrapon::GetInstance().getAtsIndexList())
		{
			panel[a] = static_cast<int>(CTrapon::GetInstance().getTimeTable(a));//時刻表
		}

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
					CATSPN::GetInstance()->HaltSound(sound);//駅通防止チャイム
					CATSPN::GetInstance()->ApproachSound(sound);//「接近，接近」
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

			//ボタンの音
			g_trapon_push(sound);
			g_trapon_release(sound);
			g_trapon_on(sound);
			g_trapon_off(sound);

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
						g_Ding1.Start();
						g_Ding1.flag = false;
					}
					else if (g_Ding2.flag)
					{
						g_Ding2.Start();
						g_Ding2.flag = false;
					}
				}
			}


			if (l_pause)//ポーズのとき
			{
				if (g_graph)g_graph.Stop();
			}
#ifdef EXCEPTION
	}
	catch (std::exception& ex)
	{
		MessageBoxA(nullptr, ex.what(), std::source_location::current().function_name(), MB_OK);
	}
	catch (winrt::hresult_error& hr)
	{
		std::wstringstream ss1,ss2;
		ss1 << L"エラーコード："<<std::hex << hr.code()<<L"\n"<< hr.message().c_str();
		ss2 << std::source_location::current().function_name();
		MessageBox(nullptr, ss1.str().c_str(), ss2.str().c_str(), MB_OK);
	}
#endif // EXCEPTION
	return ret;
}

// Called when the power is changed
ATS_API void WINAPI atsSetPower(int notch)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	g_Power = notch;
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

// Called when the brake is changed
ATS_API void WINAPI atsSetBrake(int notch)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	g_Brake = notch; 
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

// Called when the reverser is changed
ATS_API void WINAPI atsSetReverser(int pos)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	g_Reverser = pos;
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

// Called when any ATS key is pressed
ATS_API void WINAPI atsKeyDown(int atsKeyCode)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	switch (atsKeyCode)
	{
	case ATS_KEY_S:
		if (!g_Space)
		{
//			g_Ding->Stop();
//			g_Ding.reSubmitSourceBuffer();
//			g_Ding->Start();
			g_Ding0.LoopCount(nullptr);
			g_Ding0.StartTime(nullptr);
			g_Ding0.EndTime(dingDuration);
			g_Ding0.Reset();
			g_Ding0.Start();
			g_Space = true;
			g_belltimer = 0;
		}
		break;
	case ATS_KEY_A1:
		if (!g_insert_push)
		{
			if ((GetKeyState(VK_SHIFT) & 0x80)&& !(GetKeyState(VK_CONTROL) & 0x80))
			{
				bool power = CTrapon::GetInstance().powerButton();
				if (power)g_trapon_on.Start();
				else g_trapon_off.Start();
			}
			else [[likely]]
			{
				if (CDoorcontrol::GetInstance())CDoorcontrol::GetInstance()->NambaDoorOpn();
			}
			g_insert_push = true;
		}
		break;
	case ATS_KEY_A2:
		if (!g_delete_push)
		{
			if ((GetKeyState(VK_SHIFT) & 0x80) && !(GetKeyState(VK_CONTROL) & 0x80))
			{
				CTrapon::GetInstance().IcCard();
			}
			else [[likely]]
			{
				CTrapon::GetInstance().changeColor();
			}
			g_trapon_push.Start();
			g_delete_push = true;
		}
		break;
	case ATS_KEY_B1:
		if (!g_home_push)
		{
			if (CATSPN::GetInstance())CATSPN::GetInstance()->resetATSPN();
			g_trapon_push.Start();
			g_home_push = true;
		}
		break;
	case ATS_KEY_C1:
		if (!g_PgUp_push) 
		{
			if ((GetKeyState(VK_SHIFT) & 0x80) && !(GetKeyState(VK_CONTROL) & 0x80))
			{
				CTrapon::GetInstance().nextTimeTable();
			}
			g_PgUp_push = true;
		}
		break;
	case ATS_KEY_C2:
		if (!g_PgDn_push)
		{
			if ((GetKeyState(VK_SHIFT) & 0x80) && !(GetKeyState(VK_CONTROL) & 0x80))
			{
				CTrapon::GetInstance().prevTimeTable();
			}
			g_PgDn_push = true;
		}
		break;
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

// Called when any ATS key is released
ATS_API void WINAPI atsKeyUp(int atsKeyCode)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	switch (atsKeyCode)
	{
	case ATS_KEY_S:
		if (g_Space)
		{
//			g_Ding->ExitLoop();
			g_Ding.StartTime(g_Ding0.Position());
			g_Ding0.Stop();
			g_Ding.EndTime(nullptr);
			g_Ding.LoopCount(0);
			g_Ding.Reset();
			g_Ding.Start();
			g_Ding.StartTime(nullptr);
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
		if (g_home_push)
		{
			g_trapon_release.Start();
			g_home_push = false;
		}
		break;
	case ATS_KEY_A1:
		if (g_insert_push)
		{
			g_insert_push = false;
		}
		break;
	case ATS_KEY_A2:
		if (g_delete_push)
		{
			g_trapon_release.Start();
			g_delete_push = false;
		}
		break;
	case ATS_KEY_C1:
		if (g_PgUp_push)g_PgUp_push = false;
		break;
	case ATS_KEY_C2:
		if (g_PgDn_push)g_PgDn_push = false;
		break;
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

// Called when the horn is used
ATS_API void WINAPI atsHornBlow(int hornType)
{

}

// Called when the door is opened
ATS_API void WINAPI atsDoorOpen(void)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	if (CATSPN::GetInstance())CATSPN::GetInstance()->haltOFF();
	if (CDoorcontrol::GetInstance())
	{
		CDoorcontrol::GetInstance()->DoorOpn();
	}
	g_ShasyouBell = false;
	g_Bell1 = false;
	g_Bell2 = false;
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

// Called when the door is closed
ATS_API void WINAPI atsDoorClose(void)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
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

// Called when current signal is changed
ATS_API void WINAPI atsSetSignal(int signal)
{

}

// Called when the beacon data is received
ATS_API void WINAPI atsSetBeaconData(ATS_BEACONDATA beaconData)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
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
		{
			int dia = beaconData.Optional % 1000;
			int index = beaconData.Optional / 10000;
			g_timetable = index * 10000 + dia;
			CTrapon::GetInstance().addTrainNo(100 + index, dia);
		}
		CTrapon::GetInstance().wakeUp();
		if (CDoorcontrol::GetInstance())
		{
			CDoorcontrol::GetInstance()->setTrainNo(g_timetable);
		}
		else
		{
			CDoorcontrol::CreateInstance(g_module_dir);
			if (CDoorcontrol::GetInstance())
			{
				CDoorcontrol::GetInstance()->setTrainNo(g_timetable);
			}
		}
		if (beaconData.Optional % 10000 >= 1000)
		{
			if (CAutoAnnounce::GetInstance())
			{
				CAutoAnnounce::GetInstance()->setTrainNo(beaconData.Optional);
			}
			else
			{
				CAutoAnnounce::CreateInstance(g_module_dir, g_deltaT);
				if (CAutoAnnounce::GetInstance())
				{
					CAutoAnnounce::GetInstance()->setTrainNo(beaconData.Optional);
				}
			}
		}
		break;
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





