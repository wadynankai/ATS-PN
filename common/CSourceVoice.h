#ifndef CSOURCEVOICE_INCLUDED
#define CSOURCEVOICE_INCLUDED
#include <vector>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <utility>
#include <chrono>
#include <memory>
#ifndef _WIN32_WINNT
#include <winsdkver.h>
#define _WIN32_WINNT//最新バージョンのWindows
//#define _WIN32_WINNT _WIN32_WINNT_WIN10//Windows10
//#define _WIN32_WINNT _WIN32_WINNT_WINBLUE//Windows8.1
//#define _WIN32_WINNT _WIN32_WINNT_WIN8//Windows8
//#define _WIN32_WINNT _WIN32_WINNT_WIN7//Windows7
#endif //_WIN32_WINNT
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <sdkddkver.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <winrt/base.h>
#include <winrt\windows.foundation.h>


/*
プロジェクトの [プロパティページ] ダイアログボックスを開きます。
[構成プロパティ] で、[ C/c + + ] フォルダーを展開し、[コマンドライン] プロパティページをクリックします。
[追加のオプション] ボックスに、" /await " コンパイラオプションを入力します。 [OK] または [適用] を選択して、変更を保存します。*/
#if(_WIN32_WINNT>=_WIN32_WINNT_WIN8)//Windows8，8.1，10
#include <xaudio2.h>
#else//Windows7
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\XAudio2.h>
#endif

//ライブラリリンク
#if(_WIN32_WINNT<_WIN32_WINNT_WIN10&&_WIN32_WINNT>=_WIN32_WINNT_WIN8)
#pragma comment(lib,"xaudio2_8.lib")//Windows 8.1または8の時はxaudio2_8.libをリンク
#elif(_WIN32_WINNT<_WIN32_WINNT_WIN8)//Windows 7の時はXAPOFX.libをリンク
#pragma comment(lib,"XAPOFX.lib")
#endif//Windows10はライブラリリンクなし（動的リンク）

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")


//アロー演算子でアクセスできるメソッドを一部制限
class Xaudio2CSourvoiceInterface
{
public:
	//再生
	inline HRESULT Start(
		const UINT32 OperationSet = XAUDIO2_COMMIT_NOW//オペレーションセット（省略可）
	) noexcept
	{
		XAUDIO2_VOICE_STATE state = { 0 };
		GetState(&state);
		if(m_pSourceVoice)m_samplesPlayedAtStarted = state.SamplesPlayed;
		if (m_LoopCount == XAUDIO2_LOOP_INFINITE)
		{
			return StartInf(OperationSet);
		}
		else return StartBound(OperationSet);
	}
	//停止
	inline HRESULT Stop(
		const UINT32 OperationSet = XAUDIO2_COMMIT_NOW//オペレーションセット（省略可）
	) noexcept
	{
		if (m_LoopCount == XAUDIO2_LOOP_INFINITE)
		{
			return StopInf(OperationSet);
		}
		else return StopBound(OperationSet);
	}

	//音量を0から1の間で設定
	inline HRESULT SetVolume_0_1(
		const float vol,//音量（1でオリジナルの音量）
		const UINT32 OperationSet = XAUDIO2_COMMIT_NOW//オペレーションセット（省略可）
	) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->SetVolume(std::clamp(vol, 0.0f, 1.0f), OperationSet);
		else return XAUDIO2_E_INVALID_CALL;
	}
	//その他のIXaudio2SourceVoiceに実装されている関数
	inline  HRESULT DisableEffect(const UINT32 EffectIndex, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->DisableEffect(EffectIndex, OperationSet);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline 	HRESULT Discontinuity(void) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->Discontinuity();
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline 	HRESULT EnableEffect(const UINT32 EffectIndex, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->EnableEffect(EffectIndex, OperationSet);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline 	HRESULT ExitLoop(const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->ExitLoop(OperationSet);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline 	void GetChannelVolumes(const UINT32 Channels, float* pVolumes) const noexcept
	{
		if (m_pSourceVoice)m_pSourceVoice->GetChannelVolumes(Channels, pVolumes);
	}
	inline 	HRESULT GetEffectParameters(const UINT32 EffectIndex, void* pParameters, const UINT32 ParametersByteSize) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->GetEffectParameters(EffectIndex, pParameters, ParametersByteSize);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline 	void GetEffectState(const UINT32 EffectIndex, BOOL* pEnabled) const noexcept
	{
		if (m_pSourceVoice)m_pSourceVoice->GetEffectState(EffectIndex, pEnabled);
	}
	inline 	void GetFilterParameters(XAUDIO2_FILTER_PARAMETERS* pParameters) const noexcept
	{
		if (m_pSourceVoice)m_pSourceVoice->GetFilterParameters(pParameters);
	}
	inline 	void GetFrequencyRatio(float* pRatio) const noexcept
	{
		if (m_pSourceVoice)m_pSourceVoice->GetFrequencyRatio(pRatio);
	}
	inline 	void GetOutputFilterParameters(IXAudio2Voice* pDestinationVoice, XAUDIO2_FILTER_PARAMETERS* pParameters) const noexcept
	{
		if (m_pSourceVoice)m_pSourceVoice->GetOutputFilterParameters(pDestinationVoice, pParameters);
	}
	inline 	void GetOutputMatrix(IXAudio2Voice* pDestinationVoice, const UINT32 SourceChannels, const UINT32 DestinationChannels, float* pLevelMatrix) const noexcept
	{
		if (m_pSourceVoice)m_pSourceVoice->GetOutputMatrix(pDestinationVoice, SourceChannels, DestinationChannels, pLevelMatrix);
	}
	inline 	void GetState(XAUDIO2_VOICE_STATE* pVoiceState, UINT32 Flags = 0U) const noexcept
	{
		if (m_pSourceVoice)m_pSourceVoice->GetState(pVoiceState, Flags);
	}
	inline 	void GetVoiceDetails(XAUDIO2_VOICE_DETAILS* pVoiceDetails) const noexcept
	{
		if (m_pSourceVoice)m_pSourceVoice->GetVoiceDetails(pVoiceDetails);
	}
	inline 	void GetVolume(float* pVolume) const noexcept
	{
		if (m_pSourceVoice)m_pSourceVoice->GetVolume(pVolume);
	}
	inline 	HRESULT SetChannelVolumes(const UINT32 Channels, const float* pVolumes, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->SetChannelVolumes(Channels, pVolumes, OperationSet);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline 	HRESULT SetEffectChain(const XAUDIO2_EFFECT_CHAIN* pEffectChain) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->SetEffectChain(pEffectChain);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline 	HRESULT SetEffectParameters(const UINT32 EffectIndex, const void* pParameters, const UINT32 ParametersByteSize, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->SetEffectParameters(EffectIndex, pParameters, ParametersByteSize, OperationSet);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline 	HRESULT SetFilterParameters(const XAUDIO2_FILTER_PARAMETERS* pParameters, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->SetFilterParameters(pParameters, OperationSet);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline  HRESULT SetFrequencyRatio(const float ratio, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->SetFrequencyRatio(ratio, OperationSet);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline 	HRESULT SetOutputFilterParameters(IXAudio2Voice* pDestinationVoice, XAUDIO2_FILTER_PARAMETERS* pParameters, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->SetOutputFilterParameters(pDestinationVoice, pParameters, OperationSet);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline 	HRESULT SetOutputMatrix(IXAudio2Voice* pDestinationVoice, const UINT32 SourceChannels, const UINT32 DestinationChannels, const float* pLevelMatrix, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->SetOutputMatrix(pDestinationVoice, SourceChannels, DestinationChannels, pLevelMatrix, OperationSet);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline 	HRESULT SetOutputVoices(const XAUDIO2_VOICE_SENDS* pSendList) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->SetOutputVoices(pSendList);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline 	HRESULT SetSourceSampleRate(const UINT32 NewSourceSampleRate) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->SetSourceSampleRate(NewSourceSampleRate);
		else return XAUDIO2_E_INVALID_CALL;
	}
	inline  HRESULT SetVolume(const float vol, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->SetVolume(vol, OperationSet);
		else return XAUDIO2_E_INVALID_CALL;
	}
protected:
	//空のコンストラクタ
	Xaudio2CSourvoiceInterface(nullptr_t p = nullptr) noexcept {}
	//ムーブコンストラクタ
	Xaudio2CSourvoiceInterface(Xaudio2CSourvoiceInterface&& right) noexcept :
		m_pXAudio2(std::move(right.m_pXAudio2)), m_pSourceVoice(std::exchange(right.m_pSourceVoice, nullptr)),
		m_LoopCount(right.m_LoopCount), m_audioData(std::move(right.m_audioData)), m_pWfx(std::move(right.m_pWfx)),
		m_pBuffer(std::move(right.m_pBuffer)), m_started(std::exchange(right.m_started,false)), m_Flags(right.m_Flags),
		m_MaxFrequencyRatio(right.m_MaxFrequencyRatio), m_pCallback(right.m_pCallback),
		m_pSendList(right.m_pSendList), m_pEffectChain(right.m_pEffectChain), m_samplesPlayedAtStarted(right.m_samplesPlayedAtStarted){}
	//コンストラクタ
	Xaudio2CSourvoiceInterface(
		const winrt::com_ptr<IXAudio2>& Xau2,//IXAudio2インターフェースへのポインタ 
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO,//ピッチの最大値，省略した場合は2
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	) :
		m_pXAudio2(Xau2), m_pSourceVoice(nullptr),
		m_LoopCount(LoopCount), m_audioData(), m_pWfx(std::make_unique<WAVEFORMATEX>()),
		m_pBuffer(std::make_unique<XAUDIO2_BUFFER>()), m_started(false), m_Flags(Flags),
		m_MaxFrequencyRatio(MaxFrequencyRatio), m_pCallback(pCallback),
		m_pSendList(pSendList), m_pEffectChain(pEffectChain), m_samplesPlayedAtStarted(0)
	{
		*m_pWfx = { 0 };
		*m_pBuffer = { 0 };
	}
	//デストラクタ
	~Xaudio2CSourvoiceInterface()noexcept
	{
		if (m_pXAudio2)
		{
			Destroy_Voice();
		}
	}
	winrt::com_ptr<IXAudio2> m_pXAudio2;//IXAudio2へのポインタ
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;//ソースボイスへのポインタ
	std::vector<BYTE> m_audioData{};//音声データを保存する領域
	UINT32 m_LoopCount = XAUDIO2_LOOP_INFINITE;///ループカウント（1回の場合は0，2回の場合は1，…省略した場合は無限ループ
	UINT32 m_Flags = 0x0;//フラグ
	float m_MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO;//ピッチの最大値，2
	IXAudio2VoiceCallback* m_pCallback = nullptr;//コールバックへのポインタ，省略可
	XAUDIO2_VOICE_SENDS* m_pSendList = nullptr;//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
	XAUDIO2_EFFECT_CHAIN* m_pEffectChain = nullptr;//エフェクトチェーン構造体へのポインタ，省略可
	std::unique_ptr<XAUDIO2_BUFFER> m_pBuffer;//XAUDIO2_BUFFER構造体
	std::unique_ptr<WAVEFORMATEX> m_pWfx;//ソースボイスに渡す形式
	bool m_started = false;//startしてからstopするまでtrue
	UINT64 m_samplesPlayedAtStarted;
	//再生中かどうか
	[[nodiscard]] inline bool isRunning(void) const noexcept
	{
		if (m_LoopCount == XAUDIO2_LOOP_INFINITE)
		{
			return m_started;
		}
		else
		{
			XAUDIO2_VOICE_STATE state = { 0 };
			GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
			return state.BuffersQueued > 0 && m_started;
		}
	}
	void Destroy_Voice()noexcept
	{
		if (decltype(m_pSourceVoice) tmp = std::exchange(m_pSourceVoice, nullptr))
		{
			tmp->DestroyVoice();
		}
	}

private:
	//無限ループ時の再生
	inline HRESULT StartInf(const UINT32 OperationSet) noexcept
	{
		if (HRESULT hr = S_OK; m_pSourceVoice)
		{
			if (SUCCEEDED(hr = m_pSourceVoice->Start(0U, OperationSet)))m_started = true;
			return hr;
		}
		else return XAUDIO2_E_INVALID_CALL;
	}
	//再生回数有限時の再生
	inline HRESULT StartBound(const UINT32 OperationSet) noexcept
	{
		if (HRESULT hr = S_OK; m_pSourceVoice)
		{
			if (isRunning())//再生途中の場合
			{
				if (FAILED(hr = StopBound(XAUDIO2_COMMIT_NOW)))return hr;//初めから再生しなおすために，止めてバッファを消去する。(OperationSetを使わず，直ちに停止）
			}
			if (m_pBuffer)if (FAILED(hr = m_pSourceVoice->SubmitSourceBuffer(m_pBuffer.get())))return hr;//バッファをにデータを入れる。
			if (!m_started)//もし，停止していた場合，再生する。
			{
				if (SUCCEEDED(hr = m_pSourceVoice->Start(0U, OperationSet)))m_started = true;

			}
			return hr;
		}
		else return XAUDIO2_E_INVALID_CALL;
	}
	//無限ループ時の停止
	inline HRESULT StopInf(const UINT32 OperationSet) noexcept
	{
		if (HRESULT hr = S_OK; m_pSourceVoice)
		{
			if (SUCCEEDED(m_pSourceVoice->Stop(0U, OperationSet)))m_started = false;
			return hr;
		}
		else return XAUDIO2_E_INVALID_CALL;
	}
	//再生回数有限時の停止
	inline HRESULT StopBound(const UINT32 OperationSet) noexcept
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
		else return XAUDIO2_E_INVALID_CALL;
	}
};

class CSourceVoice :Xaudio2CSourvoiceInterface
{
public:
	//空のコンストラクタ
	CSourceVoice(nullptr_t p = nullptr) noexcept :Xaudio2CSourvoiceInterface(nullptr) {}
	//コピーコンストラクタ
	CSourceVoice(const CSourceVoice& right)
	{
		*this = right;
	}
	//ムーブコンストラクタ
	CSourceVoice(CSourceVoice&& right) noexcept
		:Xaudio2CSourvoiceInterface(std::move(right)), flag(right.flag) {}
	//コンストラクタ
	CSourceVoice(
		const winrt::com_ptr<IXAudio2>& Xau2,//IXAudio2インターフェースへのポインタ 
		const std::filesystem::path& name,//音声ファイルのファイル名
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO,//ピッチの最大値，省略した場合は2
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	)
		:CSourceVoice(Xau2, nullptr, nullptr, name, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain) {}
	//コンストラクタ
	CSourceVoice(
		const winrt::com_ptr<IXAudio2>& Xau2,//IXAudio2インターフェースへのポインタ 
		HRESULT* pHr,//HRESULTの値
		const std::filesystem::path& name,//音声ファイルのファイル名
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO,//ピッチの最大値，省略した場合は2
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	)
		:CSourceVoice(Xau2, pHr, nullptr, name, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain) {}
	//コンストラクタ
	CSourceVoice(
		const winrt::com_ptr<IXAudio2>& Xau2,//IXAudio2インターフェースへのポインタ 
		std::wofstream& ofs,//初期化ログファイルへのポインタ
		const std::filesystem::path& name,//音声ファイルのファイル名
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO,//ピッチの最大値，省略した場合は2
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	)
		: CSourceVoice(Xau2, nullptr, ofs, name, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain) {}
	//コンストラクタ
	CSourceVoice(
		const winrt::com_ptr<IXAudio2>& Xau2,//IXAudio2インターフェースへのポインタ 
		HRESULT* pHr,//HRESULTの値
		std::wofstream& ofs,//初期化ログファイルへのポインタ
		const std::filesystem::path& name,//音声ファイルのファイル名
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO,//ピッチの最大値，省略した場合は2
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	)
		:Xaudio2CSourvoiceInterface(Xau2, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain)
	{
		HRESULT hr = CreateSourceVoice(name, ofs.rdbuf());
		if (pHr)*pHr = hr;
	}
	//デストラクタ
	~CSourceVoice()noexcept {}
	//コンストラクタと同じ役割をする。
	inline void reset(nullptr_t p = nullptr) noexcept
	{
		if (m_started)Stop();
		Destroy_Voice();
		m_pXAudio2 = {};
		m_LoopCount = XAUDIO2_LOOP_INFINITE;
		m_audioData.clear();
		m_pWfx.reset();
		m_pBuffer.reset();
		m_started = false;
		flag = false;
		m_Flags = 0x0;
		m_MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO;
		m_pCallback = nullptr;
		m_pSendList = nullptr;
		m_pEffectChain = nullptr;
	}

	//コンストラクタと同じ役割をする。
	inline void reset(
		const winrt::com_ptr<IXAudio2>& Xau2,//IXAudio2インターフェースへのポインタ 
		const std::filesystem::path& name,//音声ファイルのファイル名
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO,//ピッチの最大値，省略した場合は2
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	) noexcept
	{
		if (m_started)Stop();
		Destroy_Voice();
		m_audioData.clear();
		if (!m_pWfx) m_pWfx = std::make_unique<WAVEFORMATEX>();
		*m_pWfx = { 0 };
		if (!m_pBuffer)m_pBuffer = std::make_unique<XAUDIO2_BUFFER>();
		*m_pBuffer = { 0 };
		m_pXAudio2 = Xau2;
		m_LoopCount = LoopCount;
		flag = false;
		m_Flags = Flags;
		m_MaxFrequencyRatio = MaxFrequencyRatio;
		m_pCallback = pCallback;
		m_pSendList = pSendList;
		m_pEffectChain = pEffectChain;
		CreateSourceVoice(name, nullptr);
	}


	//コンストラクタと同じ役割をする。
	inline void reset(
		const winrt::com_ptr<IXAudio2>& Xau2,//IXAudio2インターフェースへのポインタ 
		std::wofstream& ofs,//初期化ログファイルへのポインタ
		const std::filesystem::path& name,//音声ファイルのファイル名
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO,//ピッチの最大値，省略した場合は2
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	) noexcept
	{
		if (m_started)Stop();
		Destroy_Voice();
		m_audioData.clear();
		if (!m_pWfx) m_pWfx = std::make_unique<WAVEFORMATEX>();
		*m_pWfx = { 0 };
		if (!m_pBuffer)m_pBuffer = std::make_unique<XAUDIO2_BUFFER>();
		*m_pBuffer = { 0 };
		m_pXAudio2 = Xau2;
		m_LoopCount = LoopCount;
		flag = false;
		m_Flags = Flags;
		m_MaxFrequencyRatio = MaxFrequencyRatio;
		m_pCallback = pCallback;
		m_pSendList = pSendList;
		m_pEffectChain = pEffectChain;
		CreateSourceVoice(name, ofs.rdbuf());
	}
	//頭出し
	inline HRESULT Cue(
		const UINT32 OperationSet = XAUDIO2_COMMIT_NOW//オペレーションセット（省略可）
	) noexcept
	{
		if (HRESULT hr = S_OK; m_pSourceVoice)
		{
			if (m_started)//再生途中の場合
			{
				if (FAILED(hr = m_pSourceVoice->Stop(0U, XAUDIO2_COMMIT_NOW)))return hr;//初めから再生しなおすために，止める。(OperationSetを使わず，直ちに停止）
			}
			if (FAILED(hr = m_pSourceVoice->FlushSourceBuffers()))return hr;//バッファを消去する。
			if (m_pBuffer)if (FAILED(hr = m_pSourceVoice->SubmitSourceBuffer(m_pBuffer.get())))return hr;//バッファをにデータを入れる。
			if (m_started)//もし，直前に再生していた場合，再生する。
			{
				XAUDIO2_VOICE_STATE state = { 0 };
				GetState(&state);
				if (m_pSourceVoice)m_samplesPlayedAtStarted = state.SamplesPlayed;
				if (SUCCEEDED(hr = m_pSourceVoice->Start(0U, OperationSet)))m_started = true;
			}
			return hr;
		}
		else return XAUDIO2_E_INVALID_CALL;
	}
	//現在の音声の振幅を返す。
	[[nodiscard]] inline float getLevel(void) const noexcept
	{
		if (m_pSourceVoice)
		{
			XAUDIO2_VOICE_STATE state = { 0 };
			GetState(&state);
			if (state.BuffersQueued > 0)
			{
				size_t BufferSamples = 0;
				if (m_pWfx->wBitsPerSample != 0)BufferSamples = m_audioData.size() / (static_cast<size_t>(m_pWfx->wBitsPerSample) / bitPerByte);
				size_t sample = static_cast<size_t>(state.SamplesPlayed);
				if (BufferSamples != 0)sample %= BufferSamples;
				size_t sampleNum200Hz = m_pWfx->nSamplesPerSec / 200;
				float val = getSampleAvg(sample);
				float temp;
				for (size_t i = 0; i < sampleNum200Hz; ++i)
				{
					if (sample >= i * static_cast<int>(m_pWfx->nChannels))
					{
						temp = getSampleAvg(sample - i * static_cast<size_t>(m_pWfx->nChannels));
						if (temp > val)
						{
							val = temp;
						}
					}

				}
				return val;
			}
			else return 0.0f;
		}
		else return 0.0f;
	}
	//ソースボイスのポインタが有効かどうかを返す
	inline constexpr explicit operator bool(void) const noexcept
	{
		return m_pSourceVoice != nullptr;
	}
	inline constexpr Xaudio2CSourvoiceInterface* operator->() noexcept { return this; }
	//コピー代入
	inline CSourceVoice& operator =(const CSourceVoice& right) noexcept
	{
		if (m_started)Stop();
		Destroy_Voice();//代入するため，一旦DestroyVoice
		m_pXAudio2 = right.m_pXAudio2;
		m_LoopCount = right.m_LoopCount;
		m_audioData = right.m_audioData;
		if (!m_pWfx) m_pWfx = std::make_unique<WAVEFORMATEX>();
		*m_pWfx = *right.m_pWfx;
		if (!m_pBuffer)m_pBuffer = std::make_unique<XAUDIO2_BUFFER>();
		*m_pBuffer = *right.m_pBuffer;
		m_started = false;
		flag = false;
		m_Flags = right.m_Flags;
		m_MaxFrequencyRatio = right.m_MaxFrequencyRatio;
		m_pCallback = right.m_pCallback;
		m_pSendList = right.m_pSendList;
		m_pEffectChain = right.m_pEffectChain;
		HRESULT hr = E_FAIL;
		// ソースボイスの作成
		if (m_pXAudio2 && m_pWfx)hr = m_pXAudio2->CreateSourceVoice(&m_pSourceVoice, m_pWfx.get(), m_Flags, m_MaxFrequencyRatio, m_pCallback, m_pSendList, m_pEffectChain);
		// WAVEデータのサンプルをXAUDIO2_BUFFERに渡す。
		if (SUCCEEDED(hr) && m_pXAudio2 && m_pBuffer)
		{
			m_pBuffer->pAudioData = m_audioData.data();
			m_pBuffer->Flags = XAUDIO2_END_OF_STREAM;
			m_pBuffer->AudioBytes = static_cast<UINT32>(m_audioData.size());
			m_pBuffer->LoopCount = m_LoopCount;
			if (m_LoopCount == XAUDIO2_LOOP_INFINITE)//ループ再生の時のみ
			{
				//バッファをソースボイスに登録する。
				hr = m_pSourceVoice->SubmitSourceBuffer(m_pBuffer.get());
				Start();
				SetVolume(0.0f);
			}
		}
		else
		{
			reset(nullptr);
		}

		return *this;
	}
	//ムーブ代入(nullptrも兼ねる)
	inline CSourceVoice& operator =(CSourceVoice&& right) noexcept
	{
		if (*this != right)
		{
			if (m_started)Stop();
			Destroy_Voice();//ポインタを付け替えるため，DestroyVoice
			m_pSourceVoice = right.m_pSourceVoice;
			m_pXAudio2 = std::move(right.m_pXAudio2);
			m_LoopCount = right.m_LoopCount;
			m_audioData = std::move(right.m_audioData);
			m_pWfx = std::move(right.m_pWfx);
			m_pBuffer = std::move(right.m_pBuffer);
			m_started = std::exchange(right.m_started, false);//代入元の音声を停止することを防ぐ
			flag = right.flag;
			m_Flags = right.m_Flags;
			m_MaxFrequencyRatio = right.m_MaxFrequencyRatio;
			m_pCallback = right.m_pCallback;
			m_pSendList = right.m_pSendList;
			m_pEffectChain = right.m_pEffectChain;
		}
		return *this;
	}
	//比較演算子
	bool operator ==(CSourceVoice& right) noexcept
	{
		return this->m_pSourceVoice == right.m_pSourceVoice;
	}
	//比較演算子
	bool operator !=(CSourceVoice& right) noexcept
	{
		return !(*this == right);
	}
	//再生中かどうか
	[[nodiscard]] inline bool isRunning(void)const noexcept
	{
		return Xaudio2CSourvoiceInterface::isRunning();
	}
	//バイト数の取得
	size_t getBytes()const noexcept
	{
		return m_audioData.size();
	}
	//WAVEFORMATEX構造体の取得
	void getFormat(WAVEFORMATEX* wfx)const noexcept
	{
		if (m_pWfx && wfx)
		{
			*wfx = *m_pWfx;
		}
	}
	//XAUDIO2_BUFFER 構造体の設定
	void setPlayLength(UINT32 PlayBegin = 0U, UINT32 PlayLength = 0U, UINT32 LoopBegin = 0U, UINT32 LoopLength = 0U, UINT32 LoopCount = XAUDIO2_LOOP_INFINITE)noexcept
	{
		if (m_pBuffer)
		{
			if (m_pSourceVoice)
			{
				m_pSourceVoice->FlushSourceBuffers();
			}
			m_pBuffer->PlayBegin = PlayBegin;
			m_pBuffer->PlayLength = PlayLength;
			m_pBuffer->LoopBegin = LoopBegin;
			m_pBuffer->LoopLength = LoopLength;
			m_pBuffer->LoopCount = LoopCount;
		}
	}

	void setPlayLength(
		const winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::TimeSpan>& PlayBegin = nullptr,
		const winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::TimeSpan>& PlayLength = nullptr,
		const winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::TimeSpan>& LoopBegin = nullptr,
		const winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::TimeSpan>& LoopLength = nullptr,
		const winrt::Windows::Foundation::IReference<UINT32> LoopCount = nullptr
	) noexcept
	{
		if (m_pBuffer && m_pWfx)
		{
			if (m_pSourceVoice)
			{
				m_pSourceVoice->FlushSourceBuffers();
			}
			if (PlayBegin)m_pBuffer->PlayBegin = static_cast<UINT32>(PlayBegin.Value().count() * m_pWfx->nSamplesPerSec / winrt::Windows::Foundation::TimeSpan::period::den);
			else m_pBuffer->PlayBegin = 0U;
			if (PlayLength)m_pBuffer->PlayLength = static_cast<UINT32>(PlayLength.Value().count() * m_pWfx->nSamplesPerSec / winrt::Windows::Foundation::TimeSpan::period::den);
			else m_pBuffer->PlayLength = 0U;
			if (LoopBegin)m_pBuffer->LoopBegin = static_cast<UINT32>(LoopBegin.Value().count() * m_pWfx->nSamplesPerSec / winrt::Windows::Foundation::TimeSpan::period::den);
			else m_pBuffer->LoopBegin = 0U;
			if (LoopLength)m_pBuffer->LoopLength = static_cast<UINT32>(LoopLength.Value().count() * m_pWfx->nSamplesPerSec / winrt::Windows::Foundation::TimeSpan::period::den);
			else m_pBuffer->LoopLength = 0U;
			if (m_pBuffer)m_pBuffer->LoopCount = LoopCount.Value();
			else m_pBuffer->LoopCount = XAUDIO2_LOOP_INFINITE;
			m_LoopCount = LoopCount.Value();
		}
	}
	_NODISCARD const winrt::Windows::Foundation::TimeSpan Duration()const noexcept
	{
		using namespace winrt::Windows::Foundation;
		if (m_pWfx)
		{
			return TimeSpan(winrt::Windows::Foundation::TimeSpan::period::den * m_audioData.size() / m_pWfx->nChannels / m_pWfx->nSamplesPerSec * bitPerByte / m_pWfx->wBitsPerSample);
		}
		else return winrt::Windows::Foundation::TimeSpan::zero();
	}
	_NODISCARD const winrt::Windows::Foundation::TimeSpan Position()const noexcept
	{
		XAUDIO2_VOICE_STATE state = { 0 };
		GetState(&state);
		if (m_pSourceVoice)
		{
			if (m_LoopCount == XAUDIO2_LOOP_INFINITE)
			{
				return winrt::Windows::Foundation::TimeSpan(state.SamplesPlayed % (m_audioData.size() * bitPerByte / m_pWfx->nChannels / m_pWfx->wBitsPerSample) * winrt::Windows::Foundation::TimeSpan::period::den / m_pWfx->nSamplesPerSec);
			}
			
			return winrt::Windows::Foundation::TimeSpan((state.SamplesPlayed - m_samplesPlayedAtStarted + m_pBuffer->PlayBegin) * winrt::Windows::Foundation::TimeSpan::period::den / m_pWfx->nSamplesPerSec);			
		}
		return winrt::Windows::Foundation::TimeSpan::zero();
	}


	//XAUDIO2_BUFFER 構造体の再読み込み
	void reSubmitSourceBuffer(void)noexcept
	{
		if (m_pSourceVoice)
		{
			m_pSourceVoice->FlushSourceBuffers();
			m_pSourceVoice->SubmitSourceBuffer(m_pBuffer.get());
		}
	}
	//一時停止（m_startedとm_bufferを変更せずに停止する。）
	HRESULT pause(const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) noexcept
	{
		return m_pSourceVoice->Stop(OperationSet);
	}
	//一時停止の解除（m_startedとm_bufferを変更せず再生する。）
	HRESULT pauseDefeat(const UINT32 OperationSet = XAUDIO2_COMMIT_NOW)noexcept
	{
		return m_pSourceVoice->Start(OperationSet);
	}
	//trueの時に開始するなど（外部から自由に使う）
	bool flag = false;
private:
	void StreamEnd() { m_started = false; }
	static constexpr size_t bitPerByte = std::numeric_limits<BYTE>::digits;
	//pointから始まるサンプルの各チャンネルの平均を返す。
	 [[nodiscard]] inline float getSampleAvg(size_t index) const noexcept
	{
		if (m_pWfx)
		{
			size_t uiBytesPerSample = m_pWfx->wBitsPerSample / bitPerByte;//1サンプルのサイズ（バイト単位）
			size_t uiSamplesPlayedPerChannel = std::max<size_t>(index - 1, 0) / m_pWfx->nChannels;//サンプルの数をチャンネル数で割ったもの-1
			size_t point = uiSamplesPlayedPerChannel * m_pWfx->nChannels * uiBytesPerSample;//サンプルの一番初めの要素の位置
			point %= m_audioData.size();//サンプルの一番初めの要素の位置（2周目以降の可能性があるため，余りを求める）
			switch (m_pWfx->wBitsPerSample)
			{
			case 8:
			{
				std::vector<INT8> Samples(m_pWfx->nChannels, 0);
				for (size_t i = 0; i < m_pWfx->nChannels; ++i)
				{
					Samples.at(i) = std::bit_cast<INT8>(m_audioData.at(point + i));
				}
				int avg = std::accumulate(Samples.begin(), Samples.end(), 0) / static_cast<int>(m_pWfx->nChannels);//各チャンネルの平均値
				return std::abs(static_cast<float>(avg) / static_cast<float>(std::numeric_limits<INT8>::max()));
			}
				break;
			case 16:
			{
				std::vector<INT16> Samples(m_pWfx->nChannels, 0);
				for (size_t i = 0; i < m_pWfx->nChannels; ++i)
				{
					UINT16 temp = 0;
					for (size_t j = 0; j < uiBytesPerSample; ++j)
					{
						temp <<= bitPerByte;
						temp += m_audioData.at(point + uiBytesPerSample * i + uiBytesPerSample - 1 - j);//リトルエンディアンのため，後から読み込む
					}
					Samples.at(i) = std::bit_cast<INT16>(temp);
				}
				int avg = std::accumulate(Samples.begin(), Samples.end(), 0) / static_cast<int>(m_pWfx->nChannels);//各チャンネルの平均値
				return std::abs(static_cast<float>(avg) / static_cast<float>(std::numeric_limits<INT16>::max()));
			}
				break;
			case 24:
			{
				std::vector<INT32> Samples(m_pWfx->nChannels, 0);
				for (size_t i = 0; i < m_pWfx->nChannels; ++i)
				{
					UINT32 temp = 0;
					for (size_t j = 0; j < uiBytesPerSample; ++j)
					{
						temp <<= bitPerByte;
						temp += m_audioData.at(point + uiBytesPerSample * i + uiBytesPerSample - 1 - j);//リトルエンディアンのため，後から読み込む
					}
					temp <<= bitPerByte;//32ビットにするため，左に詰め，下位8ビットは0になる。
					Samples.at(i) = std::bit_cast<INT32>(temp);
				}
				int avg = std::accumulate(Samples.begin(), Samples.end(), 0) / static_cast<int>(m_pWfx->nChannels);//各チャンネルの平均値
				return std::abs(static_cast<float>(avg) / static_cast<float>(std::numeric_limits<INT32>::max()));//32ビット変数のため，32ビットの最大値で割る。
			}
				break;
			case 32:
			{
				std::vector<INT32> Samples(m_pWfx->nChannels, 0);
				for (size_t i = 0; i < m_pWfx->nChannels; ++i)
				{
					UINT32 temp = 0;
					for (size_t j = 0; j < uiBytesPerSample; ++j)
					{
						temp <<= bitPerByte;
						temp += m_audioData.at(point + uiBytesPerSample * i + uiBytesPerSample - 1 - j);//リトルエンディアンのため，後から読み込む
					}
					Samples.at(i) = std::bit_cast<INT32>(temp);
				}
				long long avg = std::accumulate(Samples.begin(), Samples.end(), 0ll) / static_cast<long long>(m_pWfx->nChannels);//各チャンネルの平均値
				return std::abs(static_cast<float>(avg) / static_cast<float>(std::numeric_limits<INT32>::max()));
			}
				break;
			default:
				return 1.0f;
				break;
			}
		}
		else return 0.0f;
	}
	 //コンストラクタ
	 CSourceVoice(
		 const winrt::com_ptr<IXAudio2>& Xau2,//IXAudio2インターフェースへのポインタ 
		 HRESULT* pHr,//HRESULTの値
		 std::wfilebuf* pBuf,//初期化ログファイルへのポインタ
		 const std::filesystem::path& name,//音声ファイルのファイル名
		 const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		 const UINT32 Flags = 0x0,//フラグ
		 const float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO,//ピッチの最大値，省略した場合は2
		 IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		 XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		 XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	 )
		 :Xaudio2CSourvoiceInterface(Xau2, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain)
	 {
		 HRESULT hr = CreateSourceVoice(name, pBuf);
		 if (pHr)*pHr = hr;
	 }
	//音声をバッファに読み込み，ソースボイスを作る。
	HRESULT CreateSourceVoice(
		const std::filesystem::path& name,//音声データのファイル名
		std::wfilebuf* pBuf = nullptr//初期化ログファイルへのポインタ
	)
	{
		std::filesystem::path name_normal(name.lexically_normal());
		bool mfStarted = false;//メディアファンデーションプラットフォームを初期化したらTRUEにする。（通常は最後までFALSE）
		winrt::com_ptr<IMFSourceReader> pReader;//ソースリーダー構造体
		winrt::com_ptr<IMFMediaType> pMediaType;//読み込みファイルのタイプ
		winrt::com_ptr<IMFMediaType> pOutputMediaType;//再生データのタイプ
		HRESULT hr;//COM関数の戻り値
		//https://github.com/microsoft/Windows-universal-samples/blob/master/Samples/SpatialSound/cpp/AudioFileReader.cpp

		hr = MFCreateSourceReaderFromURL(name_normal.c_str(), nullptr, pReader.put());//ファイルを開く
		if (FAILED(hr))//失敗したらMFStartupを実行してからやりなおし。
		{
			hr = MFStartup(MF_VERSION);// メディアファンデーションプラットフォームの初期化
			mfStarted = SUCCEEDED(hr);//初期化出来たらTRUEにする。
			hr = MFCreateSourceReaderFromURL(name_normal.c_str(), nullptr, pReader.put());//ファイルを開く
		}
		if (SUCCEEDED(hr))hr = pReader->SetStreamSelection(static_cast<DWORD>(MF_SOURCE_READER_ALL_STREAMS), FALSE);//FIRST_AUDIO_STREAMだけをFALSEにする
		if (SUCCEEDED(hr))hr = pReader->SetStreamSelection(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), TRUE);//FIRST_AUDIO_STREAMだけをTRUEにする
		if (SUCCEEDED(hr))hr = MFCreateMediaType(pMediaType.put());
		if (SUCCEEDED(hr))hr = pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		if (SUCCEEDED(hr))hr = pMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
		if (SUCCEEDED(hr))hr = pReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, pMediaType.get());
		if (SUCCEEDED(hr))hr = pReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), pOutputMediaType.put());
		if (SUCCEEDED(hr))hr = pReader->SetStreamSelection(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), TRUE);//FIRST_AUDIO_STREAMをTRUEにする
		WAVEFORMATEX* pWaveFormat = nullptr;//次の行で使用するポインタ
		if (SUCCEEDED(hr))hr = MFCreateWaveFormatExFromMFMediaType(pOutputMediaType.get(), &pWaveFormat, nullptr);
		if (SUCCEEDED(hr) && pWaveFormat->wFormatTag != WAVE_FORMAT_PCM && pWaveFormat->nChannels != 1)return E_INVALIDARG;//引数以上
		if (SUCCEEDED(hr))*m_pWfx = *pWaveFormat;//データのコピー
		if (pWaveFormat)CoTaskMemFree(pWaveFormat);//メモリの解放（deleteしてはいけない）
		m_audioData.resize(0);
		m_audioData.reserve(1'000'000);//1MB確保
		//WAVEデータをバッファにコピー
		while (SUCCEEDED(hr))
		{
			DWORD dwFlags = 0;//ストリームの終わりを調べるフラグ
			winrt::com_ptr<IMFSample> pSample;
			if (SUCCEEDED(hr))hr = pReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwFlags, nullptr, pSample.put());//次のサンプルを読む
			if (SUCCEEDED(hr) && (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM) != 0)break;//ストリームの終わり
			if (SUCCEEDED(hr) && pSample == nullptr)continue;//サンプルがないので頭に戻る
			winrt::com_ptr<IMFMediaBuffer> pBuf;
			if (SUCCEEDED(hr))hr = pSample->ConvertToContiguousBuffer(pBuf.put());
			DWORD dwBufSize = 0;
			BYTE* pbData = nullptr;//次の行で使用するポインタ
			if (SUCCEEDED(hr))hr = pBuf->Lock(&pbData, nullptr, &dwBufSize);
			if (SUCCEEDED(hr))
			{
				//				auto currentDataSize = m_audioData.size();
				//				m_audioData.resize(currentDataSize + dwBufSize);
				//				CopyMemory(m_audioData.data() + currentDataSize, pbData, dwBufSize);
				//				m_audioData.insert(m_audioData.cend(), pbData, pbData + dwBufSize);
				std::copy(pbData, pbData + dwBufSize, std::back_inserter(m_audioData));
				hr = pBuf->Unlock();//メモリの解放（deleteしてはいけない）
			}
		}
		m_audioData.shrink_to_fit();
		// ソースボイスの作成
		if (SUCCEEDED(hr))hr = m_pXAudio2->CreateSourceVoice(&m_pSourceVoice, m_pWfx.get(), m_Flags, m_MaxFrequencyRatio, m_pCallback, m_pSendList, m_pEffectChain);
		// WAVEデータのサンプルをXAUDIO2_BUFFERに渡す。
		if (SUCCEEDED(hr))
		{
			m_pBuffer->pAudioData = m_audioData.data();
			m_pBuffer->Flags = XAUDIO2_END_OF_STREAM;
			m_pBuffer->AudioBytes = static_cast<UINT32>(m_audioData.size());
			m_pBuffer->LoopCount = m_LoopCount;
			if (m_LoopCount == XAUDIO2_LOOP_INFINITE)//ループ再生の時のみ
			{
				//バッファをソースボイスに登録する。
				hr = m_pSourceVoice->SubmitSourceBuffer(m_pBuffer.get());
				Start();
				SetVolume(0.0f);
			}
		}
		else
		{
			reset();
		}
		if (mfStarted)MFShutdown();// メディアファンデーションプラットフォームが初期化されていたら終了

		//初期化ログの書き出し
		if (FAILED(hr))//このモータ音が見つからないまたは何らかの理由でソースボイスが作成できない。
		{
			if (pBuf)
			{
				if (pBuf->is_open())
				{
					std::basic_ostream ostr(pBuf);
					ostr << L"ソースボイスの作成失敗" << std::endl;
					ostr << L"エラー: " << std::hex << hr << std::endl;
					ostr << L"ファイル名：" << name_normal.c_str() << std::endl;
				}
			}
		}
		else//このモーター音のソースボイスを作成できた。
		{
			if (pBuf)
			{
				if (pBuf->is_open())
				{
					std::basic_ostream ostr(pBuf);
					ostr << L"ソースボイスの作成成功" << std::endl;
					ostr << L"ファイル名：" << name_normal.c_str() << std::endl;
				}
			}
		}
		return hr;
	}
};




#endif // !CSOURCEVOICE_INCLUDED