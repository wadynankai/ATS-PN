#ifndef _MAKEPATTERN_H_INCLUDED_
#define _MAKEPATTERN_H_INCLUDED_


#ifndef _WIN32_WINNT
#include <winsdkver.h>
#define _WIN32_WINNT _WIN32_WINNT_WIN10
//#define _WIN32_WINNT _WIN32_WINNT_WINBLUE
//#define _WIN32_WINNT _WIN32_WINNT_WIN8
//#define _WIN32_WINNT _WIN32_WINNT_WIN7
#endif //_WIN32_WINNT
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
#endif
#define NOMINMAX
#include <Windows.h>
#include <fstream>
#include <filesystem>
#include <limits>
#include <deque>
#include "..\..\common\CURRENT_SET.h"

#include "atsplugin.h"

inline HMODULE g_hModule;
inline std::filesystem::path g_module_dir;

inline std::deque<DISTANCE_SET> g_deqTable;
inline int g_power;//マスコンの位置
inline int g_brake;//ブレーキハンドルの位置
inline int g_reverser;//レバーサの位置
inline bool g_makingTable;//パターンテーブル作成指令
inline int g_svcBrake;//常用最大ブレーキ
inline int g_emgBrake;//常用最大ブレーキ
#endif //!_MAKEPATTERN_H_INCLUDED_