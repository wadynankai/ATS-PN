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
	BOOL mfStarted = FALSE;//���f�B�A�t�@���f�[�V�����v���b�g�t�H�[����������������TRUE�ɂ���B�i�ʏ�͍Ō�܂�FALSE�j
	Microsoft::WRL::ComPtr<IMFSourceReader>pReader;//�\�[�X���[�_�[�\����
	Microsoft::WRL::ComPtr<IMFMediaType>pMediaType;//�ǂݍ��݃t�@�C���̃^�C�v
	Microsoft::WRL::ComPtr<IMFMediaType>pOutputMediaType;//�Đ��f�[�^�̃^�C�v
	WAVEFORMATEX wfx;//�\�[�X�{�C�X�ɓn���`��
	HRESULT hr;//COM�֐��̖߂�l
	//https://github.com/microsoft/Windows-universal-samples/blob/master/Samples/SpatialSound/cpp/AudioFileReader.cpp
	hr = MFCreateSourceReaderFromURL(m_szFilename, nullptr, &pReader);//�t�@�C�����J��
	if (FAILED(hr))//���s������MFStartup�����s���Ă�����Ȃ����B
	{
		hr = MFStartup(MF_VERSION);// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[���̏�����
		mfStarted = SUCCEEDED(hr);//�������o������TRUE�ɂ���B
		hr = MFCreateSourceReaderFromURL(m_szFilename, nullptr, &pReader);//�t�@�C�����J��
	}
	if (SUCCEEDED(hr))hr = pReader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);//FIRST_AUDIO_STREAM������TRUE�ɂ���
	if (SUCCEEDED(hr))hr = pReader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);//FIRST_AUDIO_STREAM������TRUE�ɂ���
	if (SUCCEEDED(hr))hr = MFCreateMediaType(&pMediaType);
	if (SUCCEEDED(hr))hr = pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	if (SUCCEEDED(hr))hr = pMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	if (SUCCEEDED(hr))hr = pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMediaType.Get());
	if (SUCCEEDED(hr))hr = pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutputMediaType);
	if (SUCCEEDED(hr))hr = pReader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);//FIRST_AUDIO_STREAM��TRUE�ɂ���
	WAVEFORMATEX* pWaveFormat = nullptr;
	if (SUCCEEDED(hr))hr = MFCreateWaveFormatExFromMFMediaType(pOutputMediaType.Get(), &pWaveFormat, nullptr);
	if (SUCCEEDED(hr) && pWaveFormat->wFormatTag != WAVE_FORMAT_PCM && pWaveFormat->nChannels != 1)return E_INVALIDARG;//�����ȏ�
	if (SUCCEEDED(hr))CopyMemory(&wfx, pWaveFormat, sizeof(wfx));
	if (pWaveFormat)CoTaskMemFree(pWaveFormat);
	m_audioData.resize(0);
	//WAVE�f�[�^���o�b�t�@�ɃR�s�[
	while (SUCCEEDED(hr))
	{
		DWORD dwFlags = 0;//�X�g���[���̏I���𒲂ׂ�t���O
		Microsoft::WRL::ComPtr<IMFSample> pSample;
		if (SUCCEEDED(hr))hr = pReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwFlags, nullptr, &pSample);//���̃T���v����ǂ�
		if (SUCCEEDED(hr) && (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM) != 0)break;//�X�g���[���̏I���
		if (SUCCEEDED(hr) && pSample == nullptr)continue;//�T���v�����Ȃ��̂œ��ɖ߂�
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
	// �\�[�X�{�C�X�̍쐬
	if (SUCCEEDED(hr))hr = m_XAudio2->CreateSourceVoice(&m_pSourceVoice, &wfx, 0U, XAUDIO2_MAX_FREQ_RATIO);
	// WAVE�f�[�^�̃T���v����XAUDIO2_BUFFER�ɓn���B
	if (SUCCEEDED(hr))
	{
		m_buffer = { 0 };
		m_buffer.pAudioData = m_audioData.data();
		m_buffer.Flags = XAUDIO2_END_OF_STREAM;
		m_buffer.AudioBytes = static_cast<UINT32>(m_audioData.size());
		m_buffer.LoopCount = m_LoopCount;
		if (m_LoopCount == XAUDIO2_LOOP_INFINITE)//���[�v�Đ��̎��̂�
		{
			//�o�b�t�@���\�[�X�{�C�X�ɓo�^����B
			hr = m_pSourceVoice->SubmitSourceBuffer(&m_buffer);
			Start();
			SetVolume(0.0f);
		}
	}
	else if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;
	if (mfStarted)MFShutdown();// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[��������������Ă�����I��

	//���������O�̏����o��
	if (FAILED(hr))//���̃��[�^����������Ȃ��܂��͉��炩�̗��R�Ń\�[�X�{�C�X���쐬�ł��Ȃ��B
	{
		if (fp)fwprintf_s(fp, L"�\�[�X�{�C�X�̍쐬���s\n�G���[:%#X\n", hr);
		if (fp)fwprintf_s(fp, L"%ls��������܂���ł����B\n", m_szFilename);
		//						fwprintf_s(fp1, L"�A�h���X�F%p\n", motornoise[i]);
//		SAFE_DELETE(*pCSourceVoice);
	}
	else//���̃��[�^�[���̃\�[�X�{�C�X���쐬�ł����B
	{
		if (fp)fwprintf_s(fp, L"�\�[�X�{�C�X�̍쐬����\n");
		if (fp)fwprintf_s(fp, L"�t�@�C�����F%ls\n", m_szFilename);
		//						fwprintf_s(fp1, L"�A�h���X�F%p\n", motornoise[i]);
	}
	return hr;
}


CSourceVoice::operator bool() const noexcept
{
	return m_pSourceVoice;
}

