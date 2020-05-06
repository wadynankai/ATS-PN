#pragma once
#include "atsplugin.h"
#include "CATSPN.h"

#define ATS_BEACON_SPDLIM 6//PN制御速度制限
#define ATS_BEACON_STOPPATTERN 12//PN制御駅通防止
#define ATS_BEACON_SPDLIMD 16//PN制御速度制限解除
#define ATS_BEACON_END 91//終端防護
#define ATS_BEACON_TIMETABLE 100//時刻表の変更
#define ATS_BEACON_HALT 105//駅通防止チャイム
#define ATS_BEACON_LINELIMIT 130//線区最高速度

int g_svcBrake;//常用最大
int g_emgBrake;//非常ブレーキ
int g_Power;//力行ノッチ
int g_Brake;//ブレーキノッチ
int g_Reverser;//レバーサ
float g_TrainSpeed;//速度
int g_time;//時刻
int g_deltaT;//前のフレームとの時刻の差
int g_tmr;//停車駅名点滅タイマー
int g_StopSta;//停車駅名
int g_AstTimer;//アスタリスク点滅タイマー
bool g_Aster;//アスタリスク
int g_timetable;//時刻表

CATSPN g_pncontrol;

ATS_HANDLES g_output;//出力