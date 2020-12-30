// MakePattern.cpp : DLL 用にエクスポートされる関数を定義します。
//
#include "MakePatternErr.h"


// Called when this plug-in is loaded
ATS_API void Load(void)
{
	makeTableFromCsv(g_module_dir / L"AtsPnPattern.csv",&g_vecPattern);
	makeTableFromCsv(g_module_dir / L"AtsPnErrPattern.csv", &g_vecErrPatten);
}

// Called when this plug-in is unloaded
ATS_API void Dispose(void)
{
	if (!g_vecErrPatten.empty())
	{
		std::stable_sort(g_vecErrPatten.begin(), g_vecErrPatten.end(), PairLesser<DISTANCE_SET>);
		std::wofstream csv;
		csv.open(g_module_dir / L"AtsPnErrPattern.csv");
		if (csv.is_open())
		{
			csv << "ATS-PN用ブレーキパターン誤差ファイル" << std::endl;
			for (const auto& a : g_vecErrPatten)
			{
				csv << std::setprecision(std::numeric_limits<DISTANCE_SET::first_type>::max_digits10) << a.first << ","
					<< std::setprecision(std::numeric_limits<DISTANCE_SET::second_type>::max_digits10) << a.second << std::endl;
			}
			csv.close();
		}
	}
}

// Returns the version numbers of ATS plug-in
ATS_API int GetPluginVersion(void)
{
	return ATS_VERSION;
}

// Called when the train is loaded
ATS_API void SetVehicleSpec(ATS_VEHICLESPEC vehicleSpec)
{
	g_svcBrake = vehicleSpec.BrakeNotches;//常用最大ブレーキ
	g_emgBrake = g_svcBrake + 1;//非常ブレーキ
}

// Called when the game is started
ATS_API void Initialize(int brake)
{
	g_mesuring = false;
}

// Called every frame
ATS_API ATS_HANDLES Elapse(ATS_VEHICLESTATE vehicleState, int* panel, int* sound)
{
	ATS_HANDLES handle;
	g_speed = vehicleState.Speed;
	g_location = vehicleState.Location;
	handle.Power = g_power;
	handle.Reverser = g_reverser;
	handle.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;
	if (g_mesuring && g_brake != (g_svcBrake + 1))//測定指令かつ非常指令でない。
	{
		handle.Brake = g_svcBrake;//常用最大
		if (vehicleState.Speed == 0.0f)//止まったら作成終了し，ブレーキ解除。
		{
			double brakeDistance = vehicleState.Location - g_locationBegin;
			double err = brakeDistance - interpolation(g_speedBegin, g_vecPattern);
			g_vecErrPatten.emplace_back(g_speedBegin, err);
			g_mesuring = false;
		}
	}
	else handle.Brake = g_brake;//普段はハンドル位置どおり。
	return handle;
}

// Called when the power is changed
ATS_API void SetPower(int notch)
{
	g_power = notch;
}

// Called when the brake is changed
ATS_API void SetBrake(int notch)
{
	g_brake = notch;
}

// Called when the reverser is changed
ATS_API void SetReverser(int pos)
{
	g_reverser = pos;
}

// Called when any ATS key is pressed
ATS_API void KeyDown(int atsKeyCode)
{
	switch (atsKeyCode)
	{
	case ATS_KEY_B1:
		if (!g_mesuring)
		{
			g_mesuring = true;
			g_speedBegin = g_speed;
			g_locationBegin = g_location;
		}
		break;
	}
}

// Called when any ATS key is released
ATS_API void KeyUp(int atsKeyCode)
{

}

// Called when the horn is used
ATS_API void HornBlow(int hornType)
{

}

// Called when the door is opened
ATS_API void DoorOpen(void)
{

}

// Called when the door is closed
ATS_API void DoorClose(void)
{

}

// Called when current signal is changed
ATS_API void SetSignal(int signal)
{

}

// Called when the beacon data is received
ATS_API void SetBeaconData(ATS_BEACONDATA beaconData)
{

}
