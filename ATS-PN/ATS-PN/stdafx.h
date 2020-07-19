// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、
// または、参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

//ライブラリリンク
//	#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#ifndef BVE_XAudio2_WIN7
#pragma comment(lib,"xaudio2.lib")
#else
#pragma comment(lib,"XAPOFX.lib")
#endif // BVE_XAudio2_WIN7


#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
// Windows ヘッダー ファイル
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#ifndef BVE_XAudio2_WIN7
#include <xaudio2.h>
#else
#include <XAudio2.h>
#endif // !BVE_XAudio2_WIN7


// プログラムに必要な追加ヘッダーをここで参照してください
