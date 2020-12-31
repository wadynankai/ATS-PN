#include "CSourceVoice.h"
Xaudio2CSourvoiceInterface::Xaudio2CSourvoiceInterface(nullptr_t p) noexcept :
	m_pXAudio2(nullptr), m_pSourceVoice(nullptr),
	m_LoopCount(XAUDIO2_LOOP_INFINITE), m_audioData(), m_pWfx(nullptr),
	m_pBuffer(nullptr), m_started(false), m_Flags(0x0),
	m_MaxFrequencyRatio(XAUDIO2_MAX_FREQ_RATIO), m_pCallback(nullptr),
	m_pSendList(nullptr), m_pEffectChain(nullptr)
{
}
CSourceVoice::CSourceVoice(nullptr_t p) noexcept :Xaudio2CSourvoiceInterface(nullptr),flag(false)
{
}

CSourceVoice::CSourceVoice(const CSourceVoice& right)
{
	*this = right;
}
Xaudio2CSourvoiceInterface::Xaudio2CSourvoiceInterface(Xaudio2CSourvoiceInterface&& right) noexcept :
	m_pXAudio2(right.m_pXAudio2), m_pSourceVoice(right.m_pSourceVoice),
	m_LoopCount(right.m_LoopCount), m_audioData(std::move(right.m_audioData)), m_pWfx(right.m_pWfx.release()),
	m_pBuffer(right.m_pBuffer.release()), m_started(right.m_started), m_Flags(right.m_Flags),
	m_MaxFrequencyRatio(right.m_MaxFrequencyRatio), m_pCallback(right.m_pCallback),
	m_pSendList(right.m_pSendList), m_pEffectChain(right.m_pEffectChain)
{
	right.m_started = false;//������̉������~���邱�Ƃ�h��
	right.m_pSourceVoice = nullptr;//�������SourceVoice��DestroyVoice���邱�Ƃ�h��
}

CSourceVoice::CSourceVoice(CSourceVoice&& right) noexcept :Xaudio2CSourvoiceInterface(std::move(right)),flag(right.flag){}

CSourceVoice::CSourceVoice(IXAudio2* Xau2, const std::filesystem::path& name, const UINT32 LoopCount,
	const UINT32 Flags, float MaxFrequencyRatio, IXAudio2VoiceCallback* const pCallback, XAUDIO2_VOICE_SENDS* const pSendList,
	XAUDIO2_EFFECT_CHAIN* const pEffectChain)
	:CSourceVoice(Xau2, nullptr, nullptr, name, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain) {}

CSourceVoice::CSourceVoice(IXAudio2* Xau2, HRESULT* pHr, const std::filesystem::path& name, const UINT32 LoopCount,
	const UINT32 Flags, float MaxFrequencyRatio, IXAudio2VoiceCallback* const pCallback, XAUDIO2_VOICE_SENDS* const pSendList,
	XAUDIO2_EFFECT_CHAIN* const pEffectChain)
	:CSourceVoice(Xau2, pHr, nullptr, name, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain) {}

CSourceVoice::CSourceVoice(IXAudio2* Xau2, FILE* fp, const std::filesystem::path& name, const UINT32 LoopCount,
	const UINT32 Flags, float MaxFrequencyRatio, IXAudio2VoiceCallback* const pCallback, XAUDIO2_VOICE_SENDS* const pSendList,
	XAUDIO2_EFFECT_CHAIN* const pEffectChain)
	: CSourceVoice(Xau2, nullptr, fp, name, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain) {}

Xaudio2CSourvoiceInterface::Xaudio2CSourvoiceInterface(IXAudio2* Xau2, const UINT32 LoopCount,
	const UINT32 Flags, float MaxFrequencyRatio, IXAudio2VoiceCallback* const pCallback, XAUDIO2_VOICE_SENDS* const pSendList,
	XAUDIO2_EFFECT_CHAIN* const pEffectChain) :
	m_pXAudio2(Xau2), m_pSourceVoice(nullptr),
	m_LoopCount(LoopCount), m_audioData(), m_pWfx(std::make_unique<WAVEFORMATEX>()),
	m_pBuffer(std::make_unique<XAUDIO2_BUFFER>()), m_started(false), m_Flags(Flags),
	m_MaxFrequencyRatio(MaxFrequencyRatio), m_pCallback(pCallback),
	m_pSendList(pSendList), m_pEffectChain(pEffectChain)
{
	*m_pWfx = { 0 };
	*m_pBuffer = { 0 };
}

CSourceVoice::CSourceVoice(IXAudio2* Xau2, HRESULT* pHr, FILE* fp, const std::filesystem::path& name, const UINT32 LoopCount,
	const UINT32 Flags, float MaxFrequencyRatio, IXAudio2VoiceCallback* const pCallback, XAUDIO2_VOICE_SENDS* const pSendList,
	XAUDIO2_EFFECT_CHAIN* const pEffectChain) : Xaudio2CSourvoiceInterface(Xau2, LoopCount, Flags, MaxFrequencyRatio,
		pCallback, pSendList, pEffectChain), flag(false)
{
	HRESULT hr = CreateSourceVoice(name, fp);
	if (pHr)*pHr = hr;
}

Xaudio2CSourvoiceInterface::~Xaudio2CSourvoiceInterface() noexcept
{
	if (m_started)Stop();
	if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;
}

CSourceVoice::~CSourceVoice() noexcept
{
}

HRESULT CSourceVoice::CreateSourceVoice(const std::filesystem::path& name, FILE* fp)
{
	bool mfStarted = false;//���f�B�A�t�@���f�[�V�����v���b�g�t�H�[����������������TRUE�ɂ���B�i�ʏ�͍Ō�܂�FALSE�j
	winrt::com_ptr<IMFSourceReader>pReader;//�\�[�X���[�_�[�\����
	winrt::com_ptr<IMFMediaType>pMediaType;//�ǂݍ��݃t�@�C���̃^�C�v
	winrt::com_ptr<IMFMediaType>pOutputMediaType;//�Đ��f�[�^�̃^�C�v
	HRESULT hr;//COM�֐��̖߂�l
	//https://github.com/microsoft/Windows-universal-samples/blob/master/Samples/SpatialSound/cpp/AudioFileReader.cpp
	hr = MFCreateSourceReaderFromURL(name.c_str(), nullptr, pReader.put());//�t�@�C�����J��
	if (FAILED(hr))//���s������MFStartup�����s���Ă�����Ȃ����B
	{
		hr = MFStartup(MF_VERSION);// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[���̏�����
		mfStarted = SUCCEEDED(hr);//�������o������TRUE�ɂ���B
		hr = MFCreateSourceReaderFromURL(name.c_str(), nullptr, pReader.put());//�t�@�C�����J��
	}
	if (SUCCEEDED(hr))hr = pReader->SetStreamSelection(static_cast<DWORD>(MF_SOURCE_READER_ALL_STREAMS), FALSE);//FIRST_AUDIO_STREAM������FALSE�ɂ���
	if (SUCCEEDED(hr))hr = pReader->SetStreamSelection(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), TRUE);//FIRST_AUDIO_STREAM������TRUE�ɂ���
	if (SUCCEEDED(hr))hr = MFCreateMediaType(pMediaType.put());
	if (SUCCEEDED(hr))hr = pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	if (SUCCEEDED(hr))hr = pMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	if (SUCCEEDED(hr))hr = pReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, pMediaType.get());
	if (SUCCEEDED(hr))hr = pReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), pOutputMediaType.put());
	if (SUCCEEDED(hr))hr = pReader->SetStreamSelection(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), TRUE);//FIRST_AUDIO_STREAM��TRUE�ɂ���
	WAVEFORMATEX* pWaveFormat = nullptr;//���̍s�Ŏg�p����|�C���^
	if (SUCCEEDED(hr))hr = MFCreateWaveFormatExFromMFMediaType(pOutputMediaType.get(), &pWaveFormat, nullptr);
	if (SUCCEEDED(hr) && pWaveFormat->wFormatTag != WAVE_FORMAT_PCM && pWaveFormat->nChannels != 1)return E_INVALIDARG;//�����ȏ�
	if (SUCCEEDED(hr))*m_pWfx = *pWaveFormat;//�f�[�^�̃R�s�[
	if (pWaveFormat)CoTaskMemFree(pWaveFormat);//�������̉���idelete���Ă͂����Ȃ��j
	m_audioData.resize(0);
	m_audioData.reserve(1'000'000);//1MB�m��
	//WAVE�f�[�^���o�b�t�@�ɃR�s�[
	while (SUCCEEDED(hr))
	{
		DWORD dwFlags = 0;//�X�g���[���̏I���𒲂ׂ�t���O
		winrt::com_ptr<IMFSample> pSample;
		if (SUCCEEDED(hr))hr = pReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwFlags, nullptr, pSample.put());//���̃T���v����ǂ�
		if (SUCCEEDED(hr) && (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM) != 0)break;//�X�g���[���̏I���
		if (SUCCEEDED(hr) && pSample == nullptr)continue;//�T���v�����Ȃ��̂œ��ɖ߂�
		winrt::com_ptr<IMFMediaBuffer> pBuf;
		if (SUCCEEDED(hr))hr = pSample->ConvertToContiguousBuffer(pBuf.put());
		DWORD dwBufSize = 0;
		BYTE* pbData = nullptr;//���̍s�Ŏg�p����|�C���^
		if (SUCCEEDED(hr))hr = pBuf->Lock(&pbData, nullptr, &dwBufSize);
		if (SUCCEEDED(hr))
		{
			std::copy(pbData, pbData + dwBufSize, std::back_inserter(m_audioData));
			hr = pBuf->Unlock();//�������̉���idelete���Ă͂����Ȃ��j
		}
	}
	m_audioData.shrink_to_fit();
	// �\�[�X�{�C�X�̍쐬
	if (SUCCEEDED(hr))hr = m_pXAudio2->CreateSourceVoice(&m_pSourceVoice, m_pWfx.get(), m_Flags, m_MaxFrequencyRatio,m_pCallback,m_pSendList,m_pEffectChain);
	// WAVE�f�[�^�̃T���v����XAUDIO2_BUFFER�ɓn���B
	if (SUCCEEDED(hr))
	{
		m_pBuffer->pAudioData = m_audioData.data();
		m_pBuffer->Flags = XAUDIO2_END_OF_STREAM;
		m_pBuffer->AudioBytes = static_cast<UINT32>(m_audioData.size());
		m_pBuffer->LoopCount = m_LoopCount;
		if (m_LoopCount == XAUDIO2_LOOP_INFINITE)//���[�v�Đ��̎��̂�
		{
			//�o�b�t�@���\�[�X�{�C�X�ɓo�^����B
			hr = m_pSourceVoice->SubmitSourceBuffer(m_pBuffer.get());
			Start();
			SetVolume(0.0f);
		}
	}
	else
	{
		reset(nullptr);
	}
	if (mfStarted)MFShutdown();// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[��������������Ă�����I��

	//���������O�̏����o��
	if (FAILED(hr))//���̃��[�^����������Ȃ��܂��͉��炩�̗��R�Ń\�[�X�{�C�X���쐬�ł��Ȃ��B
	{
		if (fp)fprintf_s(fp, reinterpret_cast<const char*>(u8"�\�[�X�{�C�X�̍쐬���s\r\n�G���[:%#X\r\n"), hr);
		if (fp)fprintf_s(fp, reinterpret_cast<const char*>(u8"�t�@�C�����F%ls\r\n"), name.c_str());
		//						fwprintf_s(fp1, L"�A�h���X�F%p\n", motornoise[i]);
//		SAFE_DELETE(*pCSourceVoice);
	}
	else//���̃��[�^�[���̃\�[�X�{�C�X���쐬�ł����B
	{
		if (fp)fprintf_s(fp, reinterpret_cast<const char*>(u8"�\�[�X�{�C�X�̍쐬����\r\n"));
		if (fp)fprintf_s(fp, reinterpret_cast<const char*>(u8"�t�@�C�����F%ls\r\n"), name.c_str());
		//						fwprintf_s(fp1, L"�A�h���X�F%p\n", motornoise[i]);
	}
	return hr;
}



