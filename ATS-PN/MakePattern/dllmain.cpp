// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "MakePattern.h"
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		wchar_t FileName[MAX_PATH];
		GetModuleFileName(hModule, FileName, MAX_PATH);
		g_module_dir = FileName;
		g_module_dir.remove_filename();
	}
	break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

