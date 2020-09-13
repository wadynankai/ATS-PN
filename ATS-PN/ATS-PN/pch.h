// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、
// または、参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#ifndef _PCH_INCLUDED
#define _PCH_INCLUDED

#include "targetver.h"

//ライブラリリンク
//	#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#if(_WIN32_WINNT>=_WIN32_WINNT_WIN10)
#pragma comment(lib,"xaudio2.lib")
#elif(_WIN32_WINNT>=_WIN32_WINNT_WIN8)
#pragma comment(lib,"xaudio2_8.lib")
#else
#pragma comment(lib,"XAPOFX.lib")
#endif // BVE_XAudio2_WIN7

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する



// Windows ヘッダー ファイル
#define _CRT_RAND_S
#include <Windows.h>
//#include <mmsystem.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#if(_WIN32_WINNT>=_WIN32_WINNT_WIN8)
#include <xaudio2.h>
#else
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\XAudio2.h>
#endif

#include <vector>
#include <locale.h>
#include <string>
#include <fstream>
#include <algorithm>
#include <climits>
#include "atsplugin.h"
#include "CSourceVoice.h"


// プログラムに必要な追加ヘッダーをここで参照してください
#endif //_PCH_INCLUDED