#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <wrl/client.h>
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=nullptr; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=nullptr; } }
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)      { if(p) { delete (p); (p)=nullptr; } }
#endif
class CSourceVoice
{
public:
	CSourceVoice();
	CSourceVoice(IXAudio2* Xau2, std::wstring name, UINT32 LoopCount = XAUDIO2_LOOP_INFINITE);
	CSourceVoice(IXAudio2* Xau2, std::wstring name, FILE*, UINT32 LoopCount = XAUDIO2_LOOP_INFINITE);
	~CSourceVoice();
	void Setparam(IXAudio2* Xau2, std::wstring name, UINT32 LoopCount = XAUDIO2_LOOP_INFINITE);
	HRESULT CreateSourceVoice(void);
	HRESULT Start(UINT32 OperationSet = XAUDIO2_COMMIT_NOW);
	HRESULT Stop(UINT32 OperationSet = XAUDIO2_COMMIT_NOW);
	HRESULT SetFrequencyRatio(float ratio, UINT32 OperationSet = XAUDIO2_COMMIT_NOW);
	HRESULT SetVolume(float vol, UINT32 OperationSet = XAUDIO2_COMMIT_NOW);
	void CreateSound(FILE*);
	bool isRunning();//çƒê∂íÜÇ©Ç«Ç§Ç©
	bool flag = false;//trueÇÃéûÇ…äJénÇ∑ÇÈÇ»Ç«
private:
	IXAudio2* m_XAudio2 = nullptr;
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;
	LPWSTR m_szFilename = nullptr;
	std::vector<BYTE> m_audioData;
	UINT32 m_LoopCount;
	XAUDIO2_BUFFER m_buffer;
	bool m_started;//startÇµÇƒÇ©ÇÁstopÇ∑ÇÈÇ‹Ç≈true
};