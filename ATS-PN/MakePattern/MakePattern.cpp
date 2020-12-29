// MakePattern.cpp : DLL 用にエクスポートされる関数を定義します。
//
#include "MakePattern.h"


// Called when this plug-in is loaded
ATS_API void Load(void)
{

}

// Called when this plug-in is unloaded
ATS_API void Dispose(void)
{
	if (!g_deqTable.empty())
	{
		double stopLoc = g_deqTable.front().second;
		std::wofstream csv;
		csv.open(g_module_dir / L"AtsPnPattern.csv");
		if (csv.is_open())
		{
			csv << "ATS-PN用ブレーキパターンファイル" << std::endl;
			for (const auto& a : g_deqTable)
			{
				csv << std::setprecision(std::numeric_limits<DISTANCE_SET::first_type>::max_digits10) << a.first << ","
					<< std::setprecision(std::numeric_limits<DISTANCE_SET::second_type>::max_digits10) << (stopLoc - a.second) << std::endl;
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
	g_makingTable = false;
}

// Called every frame
ATS_API ATS_HANDLES Elapse(ATS_VEHICLESTATE vehicleState, int* panel, int* sound)
{
	ATS_HANDLES handle;
	static int s_iSpeed = 0;//速度の整数部分
	static int s_iSpeed_pre = 0;//前フレームでの速度の整数部分
	static DISTANCE_SET s_DSetPre{ 0.0f,0.0 };//前フレームでの速度(記録用)
	handle.Power = g_power;
	handle.Reverser = g_reverser;
	handle.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;
	s_iSpeed = static_cast<int>(vehicleState.Speed);//速度を整数に変換
	if (g_makingTable && g_brake != (g_svcBrake + 1))
	{
		handle.Brake = g_svcBrake;
		if (vehicleState.Speed == 0.0f)//止まったら作成終了し，ブレーキ解除
		{
			g_deqTable.emplace_front(vehicleState.Speed, vehicleState.Location);//停止位置を記録
			g_makingTable = false;//テーブル作成状態を解除
		}
		if (s_iSpeed < s_iSpeed_pre)//前回の記録から1km/h以上下がったらデータ作成
		{
			g_deqTable.emplace_front(s_DSetPre);
		}
	}
	else handle.Brake = g_brake;



	s_iSpeed_pre = s_iSpeed;
	s_DSetPre.first = vehicleState.Speed;
	s_DSetPre.second = vehicleState.Location;
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
	case ATS_KEY_S:
		if (!g_makingTable)
		{
			g_makingTable = true;
			g_deqTable.clear();
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

