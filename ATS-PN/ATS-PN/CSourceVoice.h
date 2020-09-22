#ifndef CSOURCEVOICE_INCLUDED
#define CSOURCEVOICE_INCLUDED

#include "pch.h"


#if(_WIN32_WINNT>=_WIN32_WINNT_WIN8)
#include <xaudio2.h>
#else
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\XAudio2.h>
#endif


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
	//コンストラクタ

	CSourceVoice(
		IXAudio2* Xau2,//IXAudio2インターフェースへのポインタ 
		const std::wstring& name,//音声ファイルのファイル名
		UINT32 LoopCount = XAUDIO2_LOOP_INFINITE//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
	);

	CSourceVoice(
		IXAudio2* Xau2,//IXAudio2インターフェースへのポインタ 
		FILE* fp,//初期化ログファイルへのポインタ
		const std::wstring& name,//音声ファイルのファイル名
		UINT32 LoopCount = XAUDIO2_LOOP_INFINITE//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
	);
	~CSourceVoice();
	//コンストラクタと同じ役割をする。
	void Setparam(
		IXAudio2* Xau2,//IXAudio2インターフェースへのポインタ 
		const std::wstring& name,//音声ファイルのファイル名
		UINT32 LoopCount = XAUDIO2_LOOP_INFINITE//ループカウント（1回の場合は0，2回の場合は1，…省略した場合は無限ループ
	);

	void Setparam(
		IXAudio2* Xau2,//IXAudio2インターフェースへのポインタ 
		FILE* fp,//初期化ログファイルへのポインタ
		const std::wstring& name,//音声ファイルのファイル名
		UINT32 LoopCount = XAUDIO2_LOOP_INFINITE//ループカウント（1回の場合は0，2回の場合は1，…省略した場合は無限ループ
	);

	//再生
	HRESULT Start(
		UINT32 OperationSet = XAUDIO2_COMMIT_NOW//オペレーションセット（省略可）
	);
	//停止
	HRESULT Stop(
		UINT32 OperationSet = XAUDIO2_COMMIT_NOW//オペレーションセット（省略可）
	);
	//ピッチを設定
	HRESULT SetFrequencyRatio(
		float ratio,//ピッチ（1でオリジナルのスピード）
		UINT32 OperationSet = XAUDIO2_COMMIT_NOW//オペレーションセット（省略可）
	);
	//音量を設定
	HRESULT SetVolume(
		float vol,//音量（1でオリジナルの音量）
		UINT32 OperationSet = XAUDIO2_COMMIT_NOW//オペレーションセット（省略可）
	);
	IXAudio2SourceVoice* pSourceVoice() const;//ソースボイスの関数を直接使いたいときに使う
	bool isRunning(void) const;//再生中かどうか
	bool flag = false;//trueの時に開始するなど（外部から自由に使う）
	explicit operator bool() const noexcept;
private:
	IXAudio2* m_XAudio2 = nullptr;//IXAudio2へのポインタ
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;//ソースボイスへのポインタ
	HRESULT StartInf(UINT32 OperationSet);//無限ループ時の再生
	HRESULT StartBound(UINT32 OperationSet);//再生回数有限時の再生
	HRESULT StopInf(UINT32 OperationSet);//無限ループ時の停止
	HRESULT StopBound(UINT32 OperationSet);//再生回数有限時の停止
	LPWSTR m_szFilename = nullptr;//音声ファイル名
	//音声をバッファに読み込み，ソースボイスを作る。
	HRESULT CreateSourceVoice(
		FILE* fp = nullptr//初期化ログファイルへのポインタ
	);
	std::vector<BYTE> m_audioData;//音声データを保存する領域
	UINT32 m_LoopCount;///ループカウント（1回の場合は0，2回の場合は1，…省略した場合は無限ループ
	XAUDIO2_BUFFER m_buffer;//XAUDIO2_BUFFER構造体
	bool m_started;//startしてからstopするまでtrue
};

inline HRESULT CSourceVoice::Start(UINT32 OperationSet)
{
	if (m_LoopCount == XAUDIO2_LOOP_INFINITE)
	{
		return StartInf(OperationSet);
	}
	else return StartBound(OperationSet);
}

inline HRESULT CSourceVoice::StartInf(UINT32 OperationSet)
{
	if (HRESULT hr = S_OK; m_pSourceVoice && !m_started)
	{
		if (SUCCEEDED(hr = m_pSourceVoice->Start(0U, OperationSet)))m_started = true;
		return hr;
	}
	else return E_FAIL;
}

inline HRESULT CSourceVoice::StartBound(UINT32 OperationSet)
{
	if (HRESULT hr = S_OK; m_pSourceVoice)
	{
		if (isRunning())//再生途中の場合
		{
			if (FAILED(hr = StopBound(XAUDIO2_COMMIT_NOW)))return hr;//初めから再生しなおすために，止めてバッファを消去する。(OperationSetを使わず，直ちに停止）
		}
		if (FAILED(hr = m_pSourceVoice->SubmitSourceBuffer(&m_buffer)))return hr;//バッファをにデータを入れる。
		if (!m_started)//もし，停止していた場合，再生する。
		{
			if (SUCCEEDED(hr = m_pSourceVoice->Start(0U, OperationSet)))m_started = true;
		}
		return hr;
	}
	else return E_FAIL;
}


inline HRESULT CSourceVoice::Stop(UINT32 OperationSet)
{
	if (m_LoopCount == XAUDIO2_LOOP_INFINITE)
	{
		return StopInf(OperationSet);
	}
	else return StopBound(OperationSet);
}


inline HRESULT CSourceVoice::StopInf(UINT32 OperationSet)
{
	if (HRESULT hr = S_OK; m_pSourceVoice && m_started)
	{
		if (SUCCEEDED(m_pSourceVoice->Stop(0U, OperationSet)))m_started = false;
		return hr;
	}
	else return E_FAIL;
}

inline HRESULT CSourceVoice::StopBound(UINT32 OperationSet)
{
	if (m_pSourceVoice)
	{
		if (HRESULT hr = S_OK; m_started)
		{
			if (SUCCEEDED(hr = m_pSourceVoice->Stop(0U, OperationSet)))m_started = false;
			else return hr;
		}
		return m_pSourceVoice->FlushSourceBuffers();
	}
	else return E_FAIL;
}


inline HRESULT CSourceVoice::SetFrequencyRatio(float ratio, UINT32 OperationSet)
{
	if (m_pSourceVoice)return m_pSourceVoice->SetFrequencyRatio(ratio, OperationSet);
	else return E_FAIL;
}


inline HRESULT CSourceVoice::SetVolume(float vol, UINT32 OperationSet)
{
	if (m_pSourceVoice)return m_pSourceVoice->SetVolume(vol, OperationSet);
	else return E_FAIL;
}

inline IXAudio2SourceVoice* CSourceVoice::pSourceVoice() const
{
	return m_pSourceVoice;
}

inline bool CSourceVoice::isRunning() const
{
	if (m_LoopCount == XAUDIO2_LOOP_INFINITE)
	{
		return m_started;
	}
	else
	{
		//	BOOL isRunning = TRUE;
		XAUDIO2_VOICE_STATE state = { 0 };
		if (m_pSourceVoice)m_pSourceVoice->GetState(&state);
		//	isRunning = (state.BuffersQueued > 0) != 0;
		//	return isRunning;
		return state.BuffersQueued > 0;
	}
}


#endif // !CSOURCEVOICE_INCLUDED