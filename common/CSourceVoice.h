#ifndef CSOURCEVOICE_INCLUDED
#define CSOURCEVOICE_INCLUDED

#ifndef _WIN32_WINNT
#include <winsdkver.h>
#define _WIN32_WINNT//最新バージョンのWindows
//#define _WIN32_WINNT _WIN32_WINNT_WIN10//Windows10
//#define _WIN32_WINNT _WIN32_WINNT_WINBLUE//Windows8.1
//#define _WIN32_WINNT _WIN32_WINNT_WIN8//Windows8
//#define _WIN32_WINNT _WIN32_WINNT_WIN7//Windows7
#endif //_WIN32_WINNT
#define NOMINMAX
#include <sdkddkver.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <array>
#include <vector>
#include <filesystem>
#include <memory>
#include <algorithm>
#include <numeric>
#include <bit>
/*
プロジェクトの [プロパティページ] ダイアログボックスを開きます。
[構成プロパティ] で、[ C/c + + ] フォルダーを展開し、[コマンドライン] プロパティページをクリックします。
[追加のオプション] ボックスに、" /await " コンパイラオプションを入力します。 [OK] または [適用] を選択して、変更を保存します。*/
#include <winrt/base.h>
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


#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	if(p)delete[] (p),(p)=nullptr
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)	if(p)delete (p),(p)=nullptr
#endif
//アロー演算子でアクセスできるメソッドを一部制限
class Xaudio2CSourvoiceInterface
{
public:
	//再生
	inline HRESULT Start(
		const UINT32 OperationSet = XAUDIO2_COMMIT_NOW//オペレーションセット（省略可）
	);
	//停止
	inline HRESULT Stop(
		const UINT32 OperationSet = XAUDIO2_COMMIT_NOW//オペレーションセット（省略可）
	);

	//音量を0から1の間で設定
	inline HRESULT SetVolume_0_1(
		const float vol,//音量（1でオリジナルの音量）
		const UINT32 OperationSet = XAUDIO2_COMMIT_NOW//オペレーションセット（省略可）
	) const;
	//その他のIXaudio2SourceVoiceに実装されている関数
	inline  HRESULT DisableEffect(const UINT32 EffectIndex, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const;
	inline 	HRESULT Discontinuity(void) const;
	inline 	HRESULT EnableEffect(const UINT32 EffectIndex, UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const;
	inline 	HRESULT ExitLoop(const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const;
	inline 	void GetChannelVolumes(const UINT32 Channels, float* pVolumes) const;
	inline 	HRESULT GetEffectParameters(const UINT32 EffectIndex, void* pParameters, const UINT32 ParametersByteSize) const;
	inline 	void GetEffectState(const UINT32 EffectIndex, BOOL* pEnabled) const;
	inline 	void GetFilterParameters(XAUDIO2_FILTER_PARAMETERS* pParameters) const;
	inline 	void GetFrequencyRatio(float* pRatio) const;
	inline 	void GetOutputFilterParameters(IXAudio2Voice* pDestinationVoice, XAUDIO2_FILTER_PARAMETERS* pParameters) const;
	inline 	void GetOutputMatrix(IXAudio2Voice* pDestinationVoice, const UINT32 SourceChannels, const UINT32 DestinationChannels, float* pLevelMatrix) const;
	inline 	void GetState(XAUDIO2_VOICE_STATE* pVoiceState, UINT32 Flags = 0U) const;
	inline 	void GetVoiceDetails(XAUDIO2_VOICE_DETAILS* pVoiceDetails) const;
	inline 	void GetVolume(float* pVolume) const;
	inline 	HRESULT SetChannelVolumes(const UINT32 Channels, const float* pVolumes, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const;
	inline 	HRESULT SetEffectChain(const XAUDIO2_EFFECT_CHAIN* pEffectChain) const;
	inline 	HRESULT SetEffectParameters(const UINT32 EffectIndex, const void* pParameters, const UINT32 ParametersByteSize, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const;
	inline 	HRESULT SetFilterParameters(const XAUDIO2_FILTER_PARAMETERS* pParameters, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const;
	inline  HRESULT SetFrequencyRatio(const float ratio, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const;
	inline 	HRESULT SetOutputFilterParameters(IXAudio2Voice* pDestinationVoice, XAUDIO2_FILTER_PARAMETERS* pParameters, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const;
	inline 	HRESULT SetOutputMatrix(IXAudio2Voice* pDestinationVoice, const UINT32 SourceChannels, const UINT32 DestinationChannels, const float* pLevelMatrix, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const;
	inline 	HRESULT SetOutputVoices(const XAUDIO2_VOICE_SENDS* pSendList) const;
	inline 	HRESULT SetSourceSampleRate(const UINT32 NewSourceSampleRate) const;
	inline  HRESULT SetVolume(const float vol, const UINT32 OperationSet = XAUDIO2_COMMIT_NOW) const;
protected:
	//空のコンストラクタ
	Xaudio2CSourvoiceInterface(nullptr_t p= nullptr) noexcept;
	//ムーブコンストラクタ
	Xaudio2CSourvoiceInterface(Xaudio2CSourvoiceInterface&& right) noexcept;
	//コンストラクタ
	Xaudio2CSourvoiceInterface(
		IXAudio2* Xau2,//IXAudio2インターフェースへのポインタ 
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//ピッチの最大値，省略した場合は1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	);
	//デストラクタ
	virtual ~Xaudio2CSourvoiceInterface();
	IXAudio2* m_pXAudio2 = nullptr;//IXAudio2へのポインタ
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;//ソースボイスへのポインタ
	std::vector<BYTE> m_audioData;//音声データを保存する領域
	UINT32 m_LoopCount;///ループカウント（1回の場合は0，2回の場合は1，…省略した場合は無限ループ
	UINT32 m_Flags = 0x0;//フラグ
	float m_MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO;//ピッチの最大値，省略した場合は1024
	IXAudio2VoiceCallback* m_pCallback = nullptr;//コールバックへのポインタ，省略可
	XAUDIO2_VOICE_SENDS* m_pSendList = nullptr;//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
	XAUDIO2_EFFECT_CHAIN* m_pEffectChain = nullptr;//エフェクトチェーン構造体へのポインタ，省略可
	std::unique_ptr<XAUDIO2_BUFFER> m_pBuffer{ nullptr };//XAUDIO2_BUFFER構造体
	std::unique_ptr<WAVEFORMATEX> m_pWfx{ nullptr };//ソースボイスに渡す形式
	bool m_started;//startしてからstopするまでtrue
	_NODISCARD bool isRunning(void) const;//再生中かどうか

private:
	inline HRESULT StartInf(const UINT32 OperationSet);//無限ループ時の再生
	inline HRESULT StartBound(const UINT32 OperationSet);//再生回数有限時の再生
	inline HRESULT StopInf(const UINT32 OperationSet);//無限ループ時の停止
	inline HRESULT StopBound(const UINT32 OperationSet);//再生回数有限時の停止
};

class CSourceVoice:Xaudio2CSourvoiceInterface
{
public:
	//空のコンストラクタ
	CSourceVoice(nullptr_t p = nullptr) noexcept;
	//コピーコンストラクタ
	CSourceVoice(const CSourceVoice& right);
	//ムーブコンストラクタ
	CSourceVoice(CSourceVoice&& right) noexcept;
	//コンストラクタ
	CSourceVoice(
		IXAudio2* Xau2,//IXAudio2インターフェースへのポインタ 
		const std::filesystem::path& name,//音声ファイルのファイル名
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//ピッチの最大値，省略した場合は1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	);
	//コンストラクタ
	CSourceVoice(
		IXAudio2* Xau2,//IXAudio2インターフェースへのポインタ 
		HRESULT* pHr,//HRESULTの値
		const std::filesystem::path& name,//音声ファイルのファイル名
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//ピッチの最大値，省略した場合は1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	);
	//コンストラクタ
	CSourceVoice(
		IXAudio2* Xau2,//IXAudio2インターフェースへのポインタ 
		FILE* fp,//初期化ログファイルへのポインタ
		const std::filesystem::path& name,//音声ファイルのファイル名
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//ピッチの最大値，省略した場合は1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	);
	//コンストラクタ
	CSourceVoice(
		IXAudio2* Xau2,//IXAudio2インターフェースへのポインタ 
		HRESULT* pHr,//HRESULTの値
		FILE* fp,//初期化ログファイルへのポインタ
		const std::filesystem::path& name,//音声ファイルのファイル名
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//ピッチの最大値，省略した場合は1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	);
	//デストラクタ
	~CSourceVoice();
	//コンストラクタと同じ役割をする。
	inline void reset(nullptr_t p = nullptr) noexcept;

	//コンストラクタと同じ役割をする。
	inline void reset(
		IXAudio2* Xau2,//IXAudio2インターフェースへのポインタ 
		const std::filesystem::path& name,//音声ファイルのファイル名
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//ピッチの最大値，省略した場合は1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	);

	//コンストラクタと同じ役割をする。
	inline void reset(
		IXAudio2* Xau2,//IXAudio2インターフェースへのポインタ 
		FILE* const fp,//初期化ログファイルへのポインタ
		const std::filesystem::path& name,//音声ファイルのファイル名
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
		const UINT32 Flags = 0x0,//フラグ
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//ピッチの最大値，省略した場合は1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//コールバックへのポインタ，省略可
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//エフェクトチェーン構造体へのポインタ，省略可
	);
	//頭出し
	inline HRESULT Cue(
		const UINT32 OperationSet = XAUDIO2_COMMIT_NOW//オペレーションセット（省略可）
	);
	//現在の音声の振幅を返す。
	_NODISCARD inline float getLevel(void) const noexcept;
	inline explicit operator bool(void) const noexcept;//ソースボイスのポインタが有効かどうかを返す
	inline constexpr Xaudio2CSourvoiceInterface* operator->() noexcept{	return this;}
	inline CSourceVoice& operator =(const CSourceVoice& right);//コピー代入
	inline CSourceVoice& operator =(CSourceVoice&& right) noexcept;//ムーブ代入
	inline CSourceVoice& operator =(const nullptr_t p) noexcept;//nullptr代入
	bool operator ==(CSourceVoice& right) noexcept;//比較演算子
	bool operator !=(CSourceVoice& right) noexcept;//比較演算子
	_NODISCARD bool isRunning(void);//再生中かどうか
	bool flag = false;//trueの時に開始するなど（外部から自由に使う）
private:
	static constexpr size_t bitPerByte = std::numeric_limits<BYTE>::digits;
	//pointから始まるサンプルの各チャンネルの平均を返す。
	_NODISCARD inline float getSampleAvg(size_t index) const noexcept;
	//音声をバッファに読み込み，ソースボイスを作る。
	HRESULT CreateSourceVoice(
		const std::filesystem::path& name,//音声データのファイル名
		FILE* fp = nullptr//初期化ログファイルへのポインタ
	);
};

inline CSourceVoice& CSourceVoice::operator=(const CSourceVoice& right)
{
	if (m_started)Stop();
	if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;//代入するため，一旦DestroyVoice
	if (!right)
	{
		m_pXAudio2 = nullptr;
		m_LoopCount = XAUDIO2_LOOP_INFINITE;
		m_audioData.clear();
		m_pWfx.reset(nullptr);
		m_pBuffer.reset(nullptr);
		m_started = false;
		flag = false;
		m_Flags = 0x0;
		m_MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO;
		m_pCallback = nullptr;
		m_pSendList = nullptr;
		m_pEffectChain = nullptr;
	}
	else
	{
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


	}
	return *this;
}

inline CSourceVoice& CSourceVoice::operator=(CSourceVoice&& right) noexcept
{
	if (*this != right)
	{
		if (m_started)Stop();
		if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;//ポインタを付け替えるため，DestroyVoice
		m_pXAudio2 = right.m_pXAudio2;
		m_pSourceVoice = right.m_pSourceVoice;
		m_LoopCount = right.m_LoopCount;
		m_audioData = std::move(right.m_audioData);
		m_pWfx.reset(right.m_pWfx.release());
		m_pBuffer.reset(right.m_pBuffer.release());
		m_started = right.m_started;
		flag = right.flag;
		m_Flags = right.m_Flags;
		m_MaxFrequencyRatio = right.m_MaxFrequencyRatio;
		m_pCallback = right.m_pCallback;
		m_pSendList = right.m_pSendList;
		m_pEffectChain = right.m_pEffectChain;

		right.m_started = false;//代入元の音声を停止することを防ぐ
		right.m_pSourceVoice = nullptr;//代入元のSourceVoiceをDestroyVoiceすることを防ぐ
	}
	return *this;
}

inline CSourceVoice& CSourceVoice::operator=(const nullptr_t ptr) noexcept
{
	if (m_started)Stop();
	if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;
	m_audioData.clear();
	m_pWfx.reset(nullptr);
	m_pBuffer.reset(nullptr);
	m_pXAudio2 = nullptr;
	m_LoopCount = XAUDIO2_LOOP_INFINITE;
	flag = false;
	m_Flags = 0x0;
	m_MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO;
	m_pCallback = nullptr;
	m_pSendList = nullptr;
	m_pEffectChain = nullptr;
	return *this;
}

inline bool CSourceVoice::operator==(CSourceVoice& right) noexcept
{
	return this->m_pSourceVoice == right.m_pSourceVoice;
}

inline bool CSourceVoice::operator!=(CSourceVoice& right) noexcept
{
	return !(*this == right);
}

inline void CSourceVoice::reset(IXAudio2* Xau2, FILE* fp, const std::filesystem::path& name, const UINT32 LoopCount,
	const UINT32 Flags, float MaxFrequencyRatio, IXAudio2VoiceCallback* const pCallback, XAUDIO2_VOICE_SENDS* const pSendList,
	XAUDIO2_EFFECT_CHAIN* const pEffectChain)
{
	if (m_started)Stop();
	if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;
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
	CreateSourceVoice(name, fp);
}

inline void CSourceVoice::reset(IXAudio2* Xau2, const std::filesystem::path& name, const UINT32 LoopCount,
	const UINT32 Flags, float MaxFrequencyRatio, IXAudio2VoiceCallback* const pCallback, XAUDIO2_VOICE_SENDS* const pSendList,
	XAUDIO2_EFFECT_CHAIN* const pEffectChain)
{
	reset(Xau2, nullptr, name, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain);
}

inline void CSourceVoice::reset(nullptr_t p) noexcept
{
	*this = p;
}

inline HRESULT Xaudio2CSourvoiceInterface::StartInf(const UINT32 OperationSet)
{
	if (HRESULT hr = S_OK; m_pSourceVoice)
	{
		if (SUCCEEDED(hr = m_pSourceVoice->Start(0U, OperationSet)))m_started = true;
		return hr;
	}
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::StartBound(const UINT32 OperationSet)
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

inline HRESULT Xaudio2CSourvoiceInterface::Start(const UINT32 OperationSet)
{
	if (m_LoopCount == XAUDIO2_LOOP_INFINITE)
	{
		return StartInf(OperationSet);
	}
	else return StartBound(OperationSet);
}

inline HRESULT Xaudio2CSourvoiceInterface::StopInf(const UINT32 OperationSet)
{
	if (HRESULT hr = S_OK; m_pSourceVoice)
	{
		if (SUCCEEDED(m_pSourceVoice->Stop(0U, OperationSet)))m_started = false;
		return hr;
	}
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::StopBound(const UINT32 OperationSet)
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

inline HRESULT Xaudio2CSourvoiceInterface::Stop(const UINT32 OperationSet)
{
	if (m_LoopCount == XAUDIO2_LOOP_INFINITE)
	{
		return StopInf(OperationSet);
	}
	else return StopBound(OperationSet);
}

inline HRESULT Xaudio2CSourvoiceInterface::SetVolume_0_1(const float vol, const UINT32 OperationSet) const
{
	if (m_pSourceVoice)return m_pSourceVoice->SetVolume(std::clamp(vol, 0.0f, 1.0f), OperationSet);
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::SetFrequencyRatio(const float ratio, const UINT32 OperationSet) const
{
	if (m_pSourceVoice)return m_pSourceVoice->SetFrequencyRatio(ratio, OperationSet);
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::SetVolume(const float vol, const UINT32 OperationSet) const
{
	if (m_pSourceVoice)return m_pSourceVoice->SetVolume(vol, OperationSet);
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::DisableEffect(const UINT32 EffectIndex, UINT32 OperationSet) const
{
	if (m_pSourceVoice)return m_pSourceVoice->DisableEffect(EffectIndex, OperationSet);
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::Discontinuity(void) const
{
	if (m_pSourceVoice)return m_pSourceVoice->Discontinuity();
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::EnableEffect(const UINT32 EffectIndex, UINT32 OperationSet) const
{
	if (m_pSourceVoice)return m_pSourceVoice->EnableEffect(EffectIndex, OperationSet);
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::ExitLoop(const UINT32 OperationSet) const
{
	if (m_pSourceVoice)return m_pSourceVoice->ExitLoop(OperationSet);
	else return XAUDIO2_E_INVALID_CALL;
}

inline void Xaudio2CSourvoiceInterface::GetChannelVolumes(const UINT32 Channels, float* pVolumes) const
{
	if (m_pSourceVoice)m_pSourceVoice->GetChannelVolumes(Channels, pVolumes);
}

inline HRESULT Xaudio2CSourvoiceInterface::GetEffectParameters(const UINT32 EffectIndex, void* pParameters, const UINT32 ParametersByteSize) const
{
	if (m_pSourceVoice)return m_pSourceVoice->GetEffectParameters(EffectIndex, pParameters, ParametersByteSize);
	else return XAUDIO2_E_INVALID_CALL;
}

inline void Xaudio2CSourvoiceInterface::GetEffectState(const UINT32 EffectIndex, BOOL* pEnabled) const
{
	if (m_pSourceVoice)m_pSourceVoice->GetEffectState(EffectIndex, pEnabled);
}

inline void Xaudio2CSourvoiceInterface::GetFilterParameters(XAUDIO2_FILTER_PARAMETERS* pParameters) const
{
	if (m_pSourceVoice)m_pSourceVoice->GetFilterParameters(pParameters);
}

inline void Xaudio2CSourvoiceInterface::GetFrequencyRatio(float* pRatio) const
{
	if (m_pSourceVoice)m_pSourceVoice->GetFrequencyRatio(pRatio);
}

inline void Xaudio2CSourvoiceInterface::GetOutputFilterParameters(IXAudio2Voice* pDestinationVoice, XAUDIO2_FILTER_PARAMETERS* pParameters) const
{
	if (m_pSourceVoice)m_pSourceVoice->GetOutputFilterParameters(pDestinationVoice, pParameters);
}

inline void Xaudio2CSourvoiceInterface::GetOutputMatrix(IXAudio2Voice* pDestinationVoice, const UINT32 SourceChannels, const UINT32 DestinationChannels, float* pLevelMatrix) const
{
	if (m_pSourceVoice)m_pSourceVoice->GetOutputMatrix(pDestinationVoice, SourceChannels, DestinationChannels, pLevelMatrix);
}

inline void Xaudio2CSourvoiceInterface::GetState(XAUDIO2_VOICE_STATE* pVoiceState, UINT32 Flags) const
{
	if (m_pSourceVoice)m_pSourceVoice->GetState(pVoiceState,Flags);
}

inline void Xaudio2CSourvoiceInterface::GetVoiceDetails(XAUDIO2_VOICE_DETAILS* pVoiceDetails) const
{
	if (m_pSourceVoice)m_pSourceVoice->GetVoiceDetails(pVoiceDetails);
}

inline void Xaudio2CSourvoiceInterface::GetVolume(float* pVolume) const
{
	if (m_pSourceVoice)m_pSourceVoice->GetVolume(pVolume);
}

inline HRESULT Xaudio2CSourvoiceInterface::SetChannelVolumes(const UINT32 Channels, const float* pVolumes, const UINT32 OperationSet) const
{
	if (m_pSourceVoice)return m_pSourceVoice->SetChannelVolumes(Channels, pVolumes, OperationSet);
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::SetEffectChain(const XAUDIO2_EFFECT_CHAIN* pEffectChain) const
{
	if (m_pSourceVoice)return m_pSourceVoice->SetEffectChain(pEffectChain);
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::SetEffectParameters(const UINT32 EffectIndex, const void* pParameters, const UINT32 ParametersByteSize, const UINT32 OperationSet) const
{
	if (m_pSourceVoice)return m_pSourceVoice->SetEffectParameters(EffectIndex, pParameters, ParametersByteSize, OperationSet);
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::SetFilterParameters(const XAUDIO2_FILTER_PARAMETERS* pParameters, const UINT32 OperationSet) const
{
	if (m_pSourceVoice)return m_pSourceVoice->SetFilterParameters(pParameters, OperationSet);
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::SetOutputFilterParameters(IXAudio2Voice* pDestinationVoice, XAUDIO2_FILTER_PARAMETERS* pParameters, const UINT32 OperationSet) const
{
	if (m_pSourceVoice)return m_pSourceVoice->SetOutputFilterParameters(pDestinationVoice, pParameters, OperationSet);
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::SetOutputMatrix(IXAudio2Voice* pDestinationVoice, const UINT32 SourceChannels, const UINT32 DestinationChannels, const float* pLevelMatrix, const UINT32 OperationSet) const
{
	if (m_pSourceVoice)return m_pSourceVoice->SetOutputMatrix(pDestinationVoice, SourceChannels, DestinationChannels, pLevelMatrix, OperationSet);
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::SetOutputVoices(const XAUDIO2_VOICE_SENDS* pSendList) const
{
	if (m_pSourceVoice)return m_pSourceVoice->SetOutputVoices(pSendList);
	else return XAUDIO2_E_INVALID_CALL;
}

inline HRESULT Xaudio2CSourvoiceInterface::SetSourceSampleRate(const UINT32 NewSourceSampleRate) const
{
	if (m_pSourceVoice)return m_pSourceVoice->SetSourceSampleRate(NewSourceSampleRate);
	else return XAUDIO2_E_INVALID_CALL;
}

_NODISCARD inline bool Xaudio2CSourvoiceInterface::isRunning() const
{
	if (m_LoopCount == XAUDIO2_LOOP_INFINITE)
	{
		return m_started;
	}
	else
	{
		XAUDIO2_VOICE_STATE state = { 0 };
		GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
		return state.BuffersQueued > 0;
	}
}

inline bool CSourceVoice::isRunning()
{
	return Xaudio2CSourvoiceInterface::isRunning();
}

inline HRESULT CSourceVoice::Cue(const UINT32 OperationSet)
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
			if (SUCCEEDED(hr = m_pSourceVoice->Start(0U, OperationSet)))m_started = true;
		}
		return hr;
	}
	else return XAUDIO2_E_INVALID_CALL;
}

_NODISCARD inline float CSourceVoice::getSampleAvg(size_t index) const noexcept
{
	if (m_pWfx)
	{
		size_t uiBytesPerSample = m_pWfx->wBitsPerSample / bitPerByte;//1サンプルのサイズ（バイト単位）
		size_t uiSamplesPlayedPerChannel = std::max<size_t>(index - 1, 0) / m_pWfx->nChannels;//サンプルの数をチャンネル数で割ったもの-1
		size_t point = uiSamplesPlayedPerChannel * m_pWfx->nChannels * uiBytesPerSample;//サンプルの一番初めの要素の位置
		point %= m_audioData.size();//サンプルの一番初めの要素の位置（2周目以降の可能性があるため，余りを求める）
		if (m_pWfx->wBitsPerSample == 8)
		{
			std::vector<INT8> Samples(m_pWfx->nChannels, 0);
			for (size_t i = 0; i < m_pWfx->nChannels; ++i)
			{
				Samples.at(i) = std::bit_cast<INT8>(m_audioData.at(point + i));
			}
			int avg = std::accumulate(Samples.begin(), Samples.end(), 0) / static_cast<int>(m_pWfx->nChannels);//各チャンネルの平均値
			return std::abs(static_cast<float>(avg) / static_cast<float>(std::numeric_limits<INT8>::max()));
		}
		else if (m_pWfx->wBitsPerSample == 16)
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
		else return 1.0f;
	}
	else return 0.0f;
}

_NODISCARD inline float CSourceVoice::getLevel(void) const noexcept
{
	if (m_pSourceVoice)
	{
		XAUDIO2_VOICE_STATE state = { 0 };
		GetState(&state);
		if (state.BuffersQueued > 0)
		{
			size_t BufferSamples = 0;
			if (m_pWfx->wBitsPerSample != 0)BufferSamples = m_audioData.size() / (m_pWfx->wBitsPerSample / bitPerByte);
			size_t sample = static_cast<size_t>(state.SamplesPlayed);
			if (BufferSamples != 0)sample %= BufferSamples;
			std::vector<float> temp;
			temp.emplace_back(getSampleAvg(sample));
			size_t sampleNum200Hz = m_pWfx->nSamplesPerSec / 200;
			for (size_t i = 0; i < sampleNum200Hz; ++i)
			{
				if (sample < i * m_pWfx->nChannels)break;//インデックスが0になってしまう場合，そこで終了
				temp.emplace_back(getSampleAvg(sample - i * m_pWfx->nChannels));
			}
			return *std::max_element(temp.cbegin(), temp.cend());
		}
		else return 0.0f;
	}
	else return 0.0f;

}

inline CSourceVoice::operator bool() const noexcept
{
	return m_pSourceVoice != nullptr;
}


#endif // !CSOURCEVOICE_INCLUDED