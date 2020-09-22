#include "pch.h"
#include "CSourceVoice.h"
CSourceVoice::CSourceVoice() :m_XAudio2(nullptr), m_pSourceVoice(nullptr), m_szFilename(nullptr), m_LoopCount(0U), m_buffer{ 0 }, m_started(false), flag(false)
{
}

CSourceVoice::CSourceVoice(IXAudio2* Xau2, const std::wstring& name, UINT32 LoopCount) :
	m_XAudio2(Xau2),
	m_pSourceVoice(nullptr),
	m_szFilename((LPWSTR)name.c_str()),
	m_LoopCount(LoopCount), m_buffer{ 0 },
	m_started(false),
	flag(false)
{
	CreateSourceVoice();
}

CSourceVoice::CSourceVoice(IXAudio2* Xau2, FILE* fp, const std::wstring& name, UINT32 LoopCount) :
	m_XAudio2(Xau2),
	m_pSourceVoice(nullptr),
	m_szFilename((LPWSTR)name.c_str()),
	m_LoopCount(LoopCount), m_buffer{ 0 },
	m_started(false),
	flag(false)
{
	CreateSourceVoice(fp);
}

void CSourceVoice::Setparam(IXAudio2* Xau2, const std::wstring& name, UINT32 LoopCount)
{
	m_XAudio2 = Xau2;
	if (m_started)Stop();
	if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;
	m_szFilename = (LPWSTR)name.c_str();
	m_audioData.clear();
	m_LoopCount = LoopCount;
	m_started = false;
	CreateSourceVoice();
}

void CSourceVoice::Setparam(IXAudio2* Xau2, FILE* fp, const std::wstring& name, UINT32 LoopCount)
{
	m_XAudio2 = Xau2;
	if (m_started)Stop();
	if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;
	m_szFilename = (LPWSTR)name.c_str();
	m_audioData.clear();
	m_LoopCount = LoopCount;
	m_started = false;
	CreateSourceVoice(fp);
}

CSourceVoice::~CSourceVoice()
{
	if (m_started)Stop();
	if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;
}


HRESULT CSourceVoice::CreateSourceVoice(FILE* fp)
{
	BOOL mfStarted = FALSE;//メディアファンデーションプラットフォームを初期化したらTRUEにする。（通常は最後までFALSE）
	Microsoft::WRL::ComPtr<IMFSourceReader>pReader;//ソースリーダー構造体
	Microsoft::WRL::ComPtr<IMFMediaType>pMediaType;//読み込みファイルのタイプ
	Microsoft::WRL::ComPtr<IMFMediaType>pOutputMediaType;//再生データのタイプ
	WAVEFORMATEX wfx;//ソースボイスに渡す形式
	HRESULT hr;//COM関数の戻り値
	//https://github.com/microsoft/Windows-universal-samples/blob/master/Samples/SpatialSound/cpp/AudioFileReader.cpp
	hr = MFCreateSourceReaderFromURL(m_szFilename, nullptr, &pReader);//ファイルを開く
	if (FAILED(hr))//失敗したらMFStartupを実行してからやりなおし。
	{
		hr = MFStartup(MF_VERSION);// メディアファンデーションプラットフォームの初期化
		mfStarted = SUCCEEDED(hr);//初期化出来たらTRUEにする。
		hr = MFCreateSourceReaderFromURL(m_szFilename, nullptr, &pReader);//ファイルを開く
	}
	if (SUCCEEDED(hr))hr = pReader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);//FIRST_AUDIO_STREAMだけをTRUEにする
	if (SUCCEEDED(hr))hr = pReader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);//FIRST_AUDIO_STREAMだけをTRUEにする
	if (SUCCEEDED(hr))hr = MFCreateMediaType(&pMediaType);
	if (SUCCEEDED(hr))hr = pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	if (SUCCEEDED(hr))hr = pMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	if (SUCCEEDED(hr))hr = pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMediaType.Get());
	if (SUCCEEDED(hr))hr = pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutputMediaType);
	if (SUCCEEDED(hr))hr = pReader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);//FIRST_AUDIO_STREAMをTRUEにする
	WAVEFORMATEX* pWaveFormat = nullptr;
	if (SUCCEEDED(hr))hr = MFCreateWaveFormatExFromMFMediaType(pOutputMediaType.Get(), &pWaveFormat, nullptr);
	if (SUCCEEDED(hr) && pWaveFormat->wFormatTag != WAVE_FORMAT_PCM && pWaveFormat->nChannels != 1)return E_INVALIDARG;//引数以上
	if (SUCCEEDED(hr))CopyMemory(&wfx, pWaveFormat, sizeof(wfx));
	if (pWaveFormat)CoTaskMemFree(pWaveFormat);
	m_audioData.resize(0);
	//WAVEデータをバッファにコピー
	while (SUCCEEDED(hr))
	{
		DWORD dwFlags = 0;//ストリームの終わりを調べるフラグ
		Microsoft::WRL::ComPtr<IMFSample> pSample;
		if (SUCCEEDED(hr))hr = pReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwFlags, nullptr, &pSample);//次のサンプルを読む
		if (SUCCEEDED(hr) && (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM) != 0)break;//ストリームの終わり
		if (SUCCEEDED(hr) && pSample == nullptr)continue;//サンプルがないので頭に戻る
		Microsoft::WRL::ComPtr<IMFMediaBuffer> pBuf;
		if (SUCCEEDED(hr))hr = pSample->ConvertToContiguousBuffer(&pBuf);
		DWORD dwBufSize = 0;
		BYTE* pbData = nullptr;
		if (SUCCEEDED(hr))hr = pBuf->Lock(&pbData, nullptr, &dwBufSize);
		size_t currentDataSize = m_audioData.size();
		m_audioData.resize(currentDataSize + dwBufSize);
		CopyMemory(m_audioData.data() + currentDataSize, pbData, dwBufSize);
		if (SUCCEEDED(hr))hr = pBuf->Unlock();
	}
	m_audioData.shrink_to_fit();
	// ソースボイスの作成
	if (SUCCEEDED(hr))hr = m_XAudio2->CreateSourceVoice(&m_pSourceVoice, &wfx, 0U, XAUDIO2_MAX_FREQ_RATIO);
	// WAVEデータのサンプルをXAUDIO2_BUFFERに渡す。
	if (SUCCEEDED(hr))
	{
		m_buffer = { 0 };
		m_buffer.pAudioData = m_audioData.data();
		m_buffer.Flags = XAUDIO2_END_OF_STREAM;
		m_buffer.AudioBytes = static_cast<UINT32>(m_audioData.size());
		m_buffer.LoopCount = m_LoopCount;
		if (m_LoopCount == XAUDIO2_LOOP_INFINITE)//ループ再生の時のみ
		{
			//バッファをソースボイスに登録する。
			hr = m_pSourceVoice->SubmitSourceBuffer(&m_buffer);
			Start();
			SetVolume(0.0f);
		}
	}
	else if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;
	if (mfStarted)MFShutdown();// メディアファンデーションプラットフォームが初期化されていたら終了

	//初期化ログの書き出し
	if (FAILED(hr))//このモータ音が見つからないまたは何らかの理由でソースボイスが作成できない。
	{
		if (fp)fwprintf_s(fp, L"ソースボイスの作成失敗\nエラー:%#X\n", hr);
		if (fp)fwprintf_s(fp, L"%lsが見つかりませんでした。\n", m_szFilename);
		//						fwprintf_s(fp1, L"アドレス：%p\n", motornoise[i]);
//		SAFE_DELETE(*pCSourceVoice);
	}
	else//このモーター音のソースボイスを作成できた。
	{
		if (fp)fwprintf_s(fp, L"ソースボイスの作成成功\n");
		if (fp)fwprintf_s(fp, L"ファイル名：%ls\n", m_szFilename);
		//						fwprintf_s(fp1, L"アドレス：%p\n", motornoise[i]);
	}
	return hr;
}


CSourceVoice::operator bool() const noexcept
{
	return m_pSourceVoice;
}

