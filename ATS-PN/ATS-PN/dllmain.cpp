// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "stdafx.h"
#include "ATS-PN.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		wchar_t FileName[MAX_PATH];
		wchar_t Drive[MAX_PATH];
		wchar_t Dir[MAX_PATH];
		GetModuleFileName(hModule, FileName, MAX_PATH);
		_wsplitpath_s(FileName, Drive, MAX_PATH, Dir, MAX_PATH, NULL, 0, NULL, 0);
		g_module_dir = (std::wstring)Drive + Dir;
		g_pncontrol.TrainSpeed = &g_TrainSpeed;
		g_pncontrol.DeltaT = &g_deltaT;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

// Called when this plug-in is loaded
ATS_API void WINAPI Load(void)
{

}

// Called when this plug-in is unloaded
ATS_API void WINAPI Dispose(void)
{
	if (g_door)delete g_door, g_door = nullptr;
	if (pMasteringVoice)pMasteringVoice->DestroyVoice();
	SAFE_RELEASE(pXAudio2);
}

// Returns the version numbers of ATS plug-in
ATS_API int WINAPI GetPluginVersion(void)
{
	return ATS_VERSION;
}

// Called when the train is loaded
ATS_API void WINAPI SetVehicleSpec(ATS_VEHICLESPEC vehicleSpec)
{
	g_svcBrake = vehicleSpec.BrakeNotches;//常用最大ブレーキ
	g_emgBrake = vehicleSpec.BrakeNotches + 1;//非常ブレーキ
}

// Called when the game is started
ATS_API void WINAPI Initialize(int brake)
{
	g_pncontrol.initATSPN();
}

// Called every frame
ATS_API ATS_HANDLES WINAPI Elapse(ATS_VEHICLESTATE vehicleState, int *panel, int *sound)
{
	g_deltaT = vehicleState.Time - g_time;
	g_time = vehicleState.Time;
	g_TrainSpeed = vehicleState.Speed;

	g_pncontrol.RunPNcontrol();//PN制御実行
	g_AstTimer += g_deltaT;
	if (g_AstTimer >= 1000)
	{
		g_Aster = !g_Aster;
		g_AstTimer %= 1000;
	}
	else if (g_AstTimer < 0)g_AstTimer = 0;

	//表示
	panel[2] = true;//GPS
	panel[3] = g_pncontrol.PatternApproachDisp;//P接近
	panel[5] = (g_pncontrol.svcBrake || g_pncontrol.emgBrake);//ブレーキ動作
	panel[6] = g_pncontrol.PNcontrolDisp;//PN制御
	panel[100] = g_timetable;//時刻表
	panel[193] = g_pncontrol.SpeedLimitDisp;//速度制限
	panel[199] = g_pncontrol.StationName;//駅名点滅
	panel[208] = g_pncontrol.haltDisp;//駅通防止
	panel[233] = g_pncontrol.TerminalSafetyDisp;//終端防護
	panel[255] = g_Aster;//アスタリスク

	//音
	sound[6] = g_pncontrol.HaltSound;//駅通防止チャイム
	sound[11] = g_pncontrol.ApproachSound;//「接近，接近」
	if (g_door)
	{
		g_door->Running(vehicleState);
		panel[217] = g_door->doorYama;
		panel[218] = g_door->doorUmi;
		sound[121] = g_door->DoorClsL;
		sound[122] = g_door->DoorClsR;
		sound[123] = g_door->DoorOpnL;
		sound[124] = g_door->DoorOpnR;
	}
	
	g_output.Reverser = g_Reverser;
	g_output.Power = g_Power;
	g_output.Brake = g_Brake;//正常時
	if (g_pncontrol.emgBrake)g_output.Brake = g_emgBrake;//非常指令
	else if (g_pncontrol.svcBrake && g_output.Brake != g_emgBrake)g_output.Brake = g_svcBrake;//常用最大指令
	g_output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;

	return g_output;
}

// Called when the power is changed
ATS_API void WINAPI SetPower(int notch)
{
	g_Power = notch;
}

// Called when the brake is changed
ATS_API void WINAPI SetBrake(int notch)
{
	g_Brake = notch;
}

// Called when the reverser is changed
ATS_API void WINAPI SetReverser(int pos)
{
	g_Reverser = pos;
}

// Called when any ATS key is pressed
ATS_API void WINAPI KeyDown(int atsKeyCode)
{

}

// Called when any ATS key is released
ATS_API void WINAPI KeyUp(int atsKeyCode)
{
	switch (atsKeyCode)
	{
	case ATS_KEY_B1:
		g_pncontrol.resetATSPN();
		break;
	case ATS_KEY_A1:
		if (g_door)g_door->NambaDoorOpn();
		break;
	}
}

// Called when the horn is used
ATS_API void WINAPI HornBlow(int hornType)
{

}

// Called when the door is opened
ATS_API void WINAPI DoorOpen(void)
{
	g_pncontrol.haltOFF();
	if (g_door)g_door->DoorOpn();
}

// Called when the door is closed
ATS_API void WINAPI DoorClose(void)
{
	g_pncontrol.haltOFF();
	if (g_door)g_door->DoorCls();
}

// Called when current signal is changed
ATS_API void WINAPI SetSignal(int signal)
{

}

// Called when the beacon data is received
ATS_API void WINAPI SetBeaconData(ATS_BEACONDATA beaconData)
{
	switch (beaconData.Type)
	{
	case ATS_BEACON_SPDLIM:
		g_pncontrol.LimitSpeedON(beaconData.Optional);
		break;
	case ATS_BEACON_STOPPATTERN:
		g_pncontrol.stopPattern(beaconData.Optional);
		break;
	case ATS_BEACON_SPDLIMD:
		g_pncontrol.LimitSpeedOFF();
		break;
	case ATS_BEACON_END:
		g_pncontrol.TerminalSafetyON(beaconData.Optional);
		break;
	case ATS_BEACON_HALT:
		g_pncontrol.haltON(beaconData.Optional);
		switch (beaconData.Optional)
		{
		case 1:
			if (g_door)g_door->NextNamba();
			break;
		case 30:
			if (g_door)g_door->NextSano();
			break;
		}
		break;
	case ATS_BEACON_LINELIMIT:
		g_pncontrol.LineMax(beaconData.Optional);
		break;
	case ATS_BEACON_TIMETABLE:
		g_timetable = beaconData.Optional % 1000;
		if (!g_door)
		{
			if (!pXAudio2)
			{
				HRESULT hr;
				if (FAILED(hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))//XAudio2初期化失敗
				{
					SAFE_RELEASE(pXAudio2)
				}
				else
				{
					if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, NULL, NULL, AudioCategory_GameEffects)))
					{
						if (pMasteringVoice)pMasteringVoice->DestroyVoice(), pMasteringVoice = nullptr;//MasteringVoiceを消す
						SAFE_RELEASE(pXAudio2);//XAudio2をやめる
					}
				}
			}
			g_door = new CDoorcontrol(g_module_dir, g_timetable, pXAudio2);
		}
		else g_door->setTrainNo(g_timetable);
		break;
	}
}

