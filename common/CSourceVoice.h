#ifndef CSOURCEVOICE_INCLUDED
#define CSOURCEVOICE_INCLUDED

#ifndef _WIN32_WINNT
#include <winsdkver.h>
#define _WIN32_WINNT//�ŐV�o�[�W������Windows
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
#include <vector>
#include <filesystem>
#include <memory>
#include <algorithm>
#include <numeric>
#include <bit>
/*
�v���W�F�N�g�� [�v���p�e�B�y�[�W] �_�C�A���O�{�b�N�X���J���܂��B
[�\���v���p�e�B] �ŁA[ C/c + + ] �t�H���_�[��W�J���A[�R�}���h���C��] �v���p�e�B�y�[�W���N���b�N���܂��B
[�ǉ��̃I�v�V����] �{�b�N�X�ɁA" /await " �R���p�C���I�v�V��������͂��܂��B [OK] �܂��� [�K�p] ��I�����āA�ύX��ۑ����܂��B*/
#include <winrt/base.h>
#if(_WIN32_WINNT>=_WIN32_WINNT_WIN8)//Windows8�C8.1�C10
#include <xaudio2.h>
#else//Windows7
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\XAudio2.h>
#endif

//���C�u���������N
#if(_WIN32_WINNT<_WIN32_WINNT_WIN10&&_WIN32_WINNT>=_WIN32_WINNT_WIN8)
#pragma comment(lib,"xaudio2_8.lib")//Windows 8.1�܂���8�̎���xaudio2_8.lib�������N
#elif(_WIN32_WINNT<_WIN32_WINNT_WIN8)//Windows 7�̎���XAPOFX.lib�������N
#pragma comment(lib,"XAPOFX.lib")
#endif//Windows10�̓��C�u���������N�Ȃ��i���I�����N�j

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")


#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	if(p)delete[] (p),(p)=nullptr
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)	if(p)delete (p),(p)=nullptr
#endif
//�A���[���Z�q�ŃA�N�Z�X�ł��郁�\�b�h���ꕔ����
class Xaudio2CSourvoiceInterface
{
public:
	//�Đ�
	inline HRESULT Start(
		const UINT32 OperationSet = XAUDIO2_COMMIT_NOW//�I�y���[�V�����Z�b�g�i�ȗ��j
	) noexcept
	{
		if (m_LoopCount == XAUDIO2_LOOP_INFINITE)
		{
			return StartInf(OperationSet);
		}
		else return StartBound(OperationSet);
	}
	//��~
	inline HRESULT Stop(
		const UINT32 OperationSet = XAUDIO2_COMMIT_NOW//�I�y���[�V�����Z�b�g�i�ȗ��j
	) noexcept
	{
		if (m_LoopCount == XAUDIO2_LOOP_INFINITE)
		{
			return StopInf(OperationSet);
		}
		else return StopBound(OperationSet);
	}

	//���ʂ�0����1�̊ԂŐݒ�
	inline HRESULT SetVolume_0_1(
		const float vol,//���ʁi1�ŃI���W�i���̉��ʁj
		const UINT32 OperationSet = XAUDIO2_COMMIT_NOW//�I�y���[�V�����Z�b�g�i�ȗ��j
	) const noexcept
	{
		if (m_pSourceVoice)return m_pSourceVoice->SetVolume(std::clamp(vol, 0.0f, 1.0f), OperationSet);
		else return XAUDIO2_E_INVALID_CALL;
	}
	//���̑���IXaudio2SourceVoice�Ɏ�������Ă���֐�
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
	//��̃R���X�g���N�^
	Xaudio2CSourvoiceInterface(nullptr_t p= nullptr) noexcept :
		m_pXAudio2(nullptr), m_pSourceVoice(nullptr),
		m_LoopCount(XAUDIO2_LOOP_INFINITE), m_audioData(), m_pWfx(nullptr),
		m_pBuffer(nullptr), m_started(false), m_Flags(0x0),
		m_MaxFrequencyRatio(XAUDIO2_MAX_FREQ_RATIO), m_pCallback(nullptr),
		m_pSendList(nullptr), m_pEffectChain(nullptr){}
	//���[�u�R���X�g���N�^
	Xaudio2CSourvoiceInterface(Xaudio2CSourvoiceInterface&& right) noexcept :
		m_pXAudio2(right.m_pXAudio2), m_pSourceVoice(right.m_pSourceVoice),
		m_LoopCount(right.m_LoopCount), m_audioData(std::move(right.m_audioData)), m_pWfx(right.m_pWfx.release()),
		m_pBuffer(right.m_pBuffer.release()), m_started(right.m_started), m_Flags(right.m_Flags),
		m_MaxFrequencyRatio(right.m_MaxFrequencyRatio), m_pCallback(right.m_pCallback),
		m_pSendList(right.m_pSendList), m_pEffectChain(right.m_pEffectChain)
	{
		right.m_started = false;//������̉������~���邱�Ƃ�h��
		right.m_pSourceVoice = nullptr;//�������SourceVoice��DestroyVoice���邱�Ƃ�h��
	}
	//�R���X�g���N�^
	Xaudio2CSourvoiceInterface(
		IXAudio2* Xau2,//IXAudio2�C���^�[�t�F�[�X�ւ̃|�C���^ 
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�C�ȗ������ꍇ�͖������[�v
		const UINT32 Flags = 0x0,//�t���O
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//�s�b�`�̍ő�l�C�ȗ������ꍇ��1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//�R�[���o�b�N�ւ̃|�C���^�C�ȗ���
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS�\���̂ւ̃|�C���^�C�ȗ���
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//�G�t�F�N�g�`�F�[���\���̂ւ̃|�C���^�C�ȗ���
	) :
		m_pXAudio2(Xau2), m_pSourceVoice(nullptr),
		m_LoopCount(LoopCount), m_audioData(), m_pWfx(std::make_unique<WAVEFORMATEX>()),
		m_pBuffer(std::make_unique<XAUDIO2_BUFFER>()), m_started(false), m_Flags(Flags),
		m_MaxFrequencyRatio(MaxFrequencyRatio), m_pCallback(pCallback),
		m_pSendList(pSendList), m_pEffectChain(pEffectChain)
	{
		*m_pWfx = { 0 };
		*m_pBuffer = { 0 };
	}
	//�f�X�g���N�^
	~Xaudio2CSourvoiceInterface() noexcept
	{
		if (m_started)Stop();
		if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;
	}
	IXAudio2* m_pXAudio2 = nullptr;//IXAudio2�ւ̃|�C���^
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;//�\�[�X�{�C�X�ւ̃|�C���^
	std::vector<BYTE> m_audioData;//�����f�[�^��ۑ�����̈�
	UINT32 m_LoopCount;///���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�ȗ������ꍇ�͖������[�v
	UINT32 m_Flags = 0x0;//�t���O
	float m_MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO;//�s�b�`�̍ő�l�C�ȗ������ꍇ��1024
	IXAudio2VoiceCallback* m_pCallback = nullptr;//�R�[���o�b�N�ւ̃|�C���^�C�ȗ���
	XAUDIO2_VOICE_SENDS* m_pSendList = nullptr;//XAUDIO2_VOICE_SENDS�\���̂ւ̃|�C���^�C�ȗ���
	XAUDIO2_EFFECT_CHAIN* m_pEffectChain = nullptr;//�G�t�F�N�g�`�F�[���\���̂ւ̃|�C���^�C�ȗ���
	std::unique_ptr<XAUDIO2_BUFFER> m_pBuffer{ nullptr };//XAUDIO2_BUFFER�\����
	std::unique_ptr<WAVEFORMATEX> m_pWfx{ nullptr };//�\�[�X�{�C�X�ɓn���`��
	bool m_started;//start���Ă���stop����܂�true
	//�Đ������ǂ���
	_NODISCARD inline bool isRunning(void) const noexcept
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

private:
	//�������[�v���̍Đ�
	inline HRESULT StartInf(const UINT32 OperationSet) noexcept
	{
		if (HRESULT hr = S_OK; m_pSourceVoice)
		{
			if (SUCCEEDED(hr = m_pSourceVoice->Start(0U, OperationSet)))m_started = true;
			return hr;
		}
		else return XAUDIO2_E_INVALID_CALL;
	}
	//�Đ��񐔗L�����̍Đ�
	inline HRESULT StartBound(const UINT32 OperationSet) noexcept
	{
		if (HRESULT hr = S_OK; m_pSourceVoice)
		{
			if (isRunning())//�Đ��r���̏ꍇ
			{
				if (FAILED(hr = StopBound(XAUDIO2_COMMIT_NOW)))return hr;//���߂���Đ����Ȃ������߂ɁC�~�߂ăo�b�t�@����������B(OperationSet���g�킸�C�����ɒ�~�j
			}
			if (m_pBuffer)if (FAILED(hr = m_pSourceVoice->SubmitSourceBuffer(m_pBuffer.get())))return hr;//�o�b�t�@���Ƀf�[�^������B
			if (!m_started)//�����C��~���Ă����ꍇ�C�Đ�����B
			{
				if (SUCCEEDED(hr = m_pSourceVoice->Start(0U, OperationSet)))m_started = true;
			}
			return hr;
		}
		else return XAUDIO2_E_INVALID_CALL;
	}
	//�������[�v���̒�~
	inline HRESULT StopInf(const UINT32 OperationSet) noexcept
	{
		if (HRESULT hr = S_OK; m_pSourceVoice)
		{
			if (SUCCEEDED(m_pSourceVoice->Stop(0U, OperationSet)))m_started = false;
			return hr;
		}
		else return XAUDIO2_E_INVALID_CALL;
	}
	//�Đ��񐔗L�����̒�~
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

class CSourceVoice:Xaudio2CSourvoiceInterface
{
public:
	//��̃R���X�g���N�^
	CSourceVoice(nullptr_t p = nullptr) noexcept :Xaudio2CSourvoiceInterface(nullptr), flag(false){}
	//�R�s�[�R���X�g���N�^
	CSourceVoice(const CSourceVoice& right)
	{
		*this = right;
	}
	//���[�u�R���X�g���N�^
	CSourceVoice(CSourceVoice&& right) noexcept
		:Xaudio2CSourvoiceInterface(std::move(right)), flag(right.flag) {}
	//�R���X�g���N�^
	CSourceVoice(
		IXAudio2* Xau2,//IXAudio2�C���^�[�t�F�[�X�ւ̃|�C���^ 
		const std::filesystem::path& name,//�����t�@�C���̃t�@�C����
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�C�ȗ������ꍇ�͖������[�v
		const UINT32 Flags = 0x0,//�t���O
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//�s�b�`�̍ő�l�C�ȗ������ꍇ��1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//�R�[���o�b�N�ւ̃|�C���^�C�ȗ���
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS�\���̂ւ̃|�C���^�C�ȗ���
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//�G�t�F�N�g�`�F�[���\���̂ւ̃|�C���^�C�ȗ���
	)
		:CSourceVoice(Xau2, nullptr, nullptr, name, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain) {}
	//�R���X�g���N�^
	CSourceVoice(
		IXAudio2* Xau2,//IXAudio2�C���^�[�t�F�[�X�ւ̃|�C���^ 
		HRESULT* pHr,//HRESULT�̒l
		const std::filesystem::path& name,//�����t�@�C���̃t�@�C����
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�C�ȗ������ꍇ�͖������[�v
		const UINT32 Flags = 0x0,//�t���O
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//�s�b�`�̍ő�l�C�ȗ������ꍇ��1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//�R�[���o�b�N�ւ̃|�C���^�C�ȗ���
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS�\���̂ւ̃|�C���^�C�ȗ���
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//�G�t�F�N�g�`�F�[���\���̂ւ̃|�C���^�C�ȗ���
	)
		:CSourceVoice(Xau2, pHr, nullptr, name, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain) {}
	//�R���X�g���N�^
	CSourceVoice(
		IXAudio2* Xau2,//IXAudio2�C���^�[�t�F�[�X�ւ̃|�C���^ 
		FILE* fp,//���������O�t�@�C���ւ̃|�C���^
		const std::filesystem::path& name,//�����t�@�C���̃t�@�C����
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�C�ȗ������ꍇ�͖������[�v
		const UINT32 Flags = 0x0,//�t���O
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//�s�b�`�̍ő�l�C�ȗ������ꍇ��1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//�R�[���o�b�N�ւ̃|�C���^�C�ȗ���
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS�\���̂ւ̃|�C���^�C�ȗ���
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//�G�t�F�N�g�`�F�[���\���̂ւ̃|�C���^�C�ȗ���
	)
		: CSourceVoice(Xau2, nullptr, fp, name, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain) {}
	//�R���X�g���N�^
	CSourceVoice(
		IXAudio2* Xau2,//IXAudio2�C���^�[�t�F�[�X�ւ̃|�C���^ 
		HRESULT* pHr,//HRESULT�̒l
		FILE* fp,//���������O�t�@�C���ւ̃|�C���^
		const std::filesystem::path& name,//�����t�@�C���̃t�@�C����
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�C�ȗ������ꍇ�͖������[�v
		const UINT32 Flags = 0x0,//�t���O
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//�s�b�`�̍ő�l�C�ȗ������ꍇ��1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//�R�[���o�b�N�ւ̃|�C���^�C�ȗ���
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS�\���̂ւ̃|�C���^�C�ȗ���
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//�G�t�F�N�g�`�F�[���\���̂ւ̃|�C���^�C�ȗ���
	)
		:Xaudio2CSourvoiceInterface(Xau2, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain), flag(false)
	{
		HRESULT hr = CreateSourceVoice(name, fp);
		if (pHr)*pHr = hr;
	}
	//�f�X�g���N�^
	~CSourceVoice() noexcept {}
	//�R���X�g���N�^�Ɠ�������������B
	inline void reset(nullptr_t p = nullptr) noexcept
	{
		*this = p;
	}

	//�R���X�g���N�^�Ɠ�������������B
	inline void reset(
		IXAudio2* Xau2,//IXAudio2�C���^�[�t�F�[�X�ւ̃|�C���^ 
		const std::filesystem::path& name,//�����t�@�C���̃t�@�C����
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�C�ȗ������ꍇ�͖������[�v
		const UINT32 Flags = 0x0,//�t���O
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//�s�b�`�̍ő�l�C�ȗ������ꍇ��1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//�R�[���o�b�N�ւ̃|�C���^�C�ȗ���
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS�\���̂ւ̃|�C���^�C�ȗ���
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//�G�t�F�N�g�`�F�[���\���̂ւ̃|�C���^�C�ȗ���
	) noexcept
	{
		reset(Xau2, nullptr, name, LoopCount, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain);
	}


	//�R���X�g���N�^�Ɠ�������������B
	inline void reset(
		IXAudio2* Xau2,//IXAudio2�C���^�[�t�F�[�X�ւ̃|�C���^ 
		FILE* const fp,//���������O�t�@�C���ւ̃|�C���^
		const std::filesystem::path& name,//�����t�@�C���̃t�@�C����
		const UINT32 LoopCount = XAUDIO2_LOOP_INFINITE,//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�C�ȗ������ꍇ�͖������[�v
		const UINT32 Flags = 0x0,//�t���O
		const float MaxFrequencyRatio = XAUDIO2_MAX_FREQ_RATIO,//�s�b�`�̍ő�l�C�ȗ������ꍇ��1024
		IXAudio2VoiceCallback* const pCallback = nullptr,//�R�[���o�b�N�ւ̃|�C���^�C�ȗ���
		XAUDIO2_VOICE_SENDS* const pSendList = nullptr,//XAUDIO2_VOICE_SENDS�\���̂ւ̃|�C���^�C�ȗ���
		XAUDIO2_EFFECT_CHAIN* const pEffectChain = nullptr//�G�t�F�N�g�`�F�[���\���̂ւ̃|�C���^�C�ȗ���
	) noexcept
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
	//���o��
	inline HRESULT Cue(
		const UINT32 OperationSet = XAUDIO2_COMMIT_NOW//�I�y���[�V�����Z�b�g�i�ȗ��j
	) noexcept 
	{
		if (HRESULT hr = S_OK; m_pSourceVoice)
		{
			if (m_started)//�Đ��r���̏ꍇ
			{
				if (FAILED(hr = m_pSourceVoice->Stop(0U, XAUDIO2_COMMIT_NOW)))return hr;//���߂���Đ����Ȃ������߂ɁC�~�߂�B(OperationSet���g�킸�C�����ɒ�~�j
			}
			if (FAILED(hr = m_pSourceVoice->FlushSourceBuffers()))return hr;//�o�b�t�@����������B
			if (m_pBuffer)if (FAILED(hr = m_pSourceVoice->SubmitSourceBuffer(m_pBuffer.get())))return hr;//�o�b�t�@���Ƀf�[�^������B
			if (m_started)//�����C���O�ɍĐ����Ă����ꍇ�C�Đ�����B
			{
				if (SUCCEEDED(hr = m_pSourceVoice->Start(0U, OperationSet)))m_started = true;
			}
			return hr;
		}
		else return XAUDIO2_E_INVALID_CALL;
	}
	//���݂̉����̐U����Ԃ��B
	_NODISCARD inline float getLevel(void) const noexcept
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
#pragma loop(hint_parallel(8))
				for (size_t i = 0; i < sampleNum200Hz; ++i)
				{
					if (sample < i * m_pWfx->nChannels)break;//�C���f�b�N�X��0�ɂȂ��Ă��܂��ꍇ�C�����ŏI��
					temp.emplace_back(getSampleAvg(sample - i * m_pWfx->nChannels));
				}
				return *std::max_element(temp.cbegin(), temp.cend());
			}
			else return 0.0f;
		}
		else return 0.0f;

	}
	//�\�[�X�{�C�X�̃|�C���^���L�����ǂ�����Ԃ�
	inline constexpr explicit operator bool(void) const noexcept
	{
		return m_pSourceVoice != nullptr;
	}
	inline constexpr Xaudio2CSourvoiceInterface* operator->() noexcept{	return this;}
	//�R�s�[���
	inline CSourceVoice& operator =(const CSourceVoice& right) noexcept
	{
		if (m_started)Stop();
		if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;//������邽�߁C��UDestroyVoice
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
			// �\�[�X�{�C�X�̍쐬
			if (m_pXAudio2 && m_pWfx)hr = m_pXAudio2->CreateSourceVoice(&m_pSourceVoice, m_pWfx.get(), m_Flags, m_MaxFrequencyRatio, m_pCallback, m_pSendList, m_pEffectChain);
			// WAVE�f�[�^�̃T���v����XAUDIO2_BUFFER�ɓn���B
			if (SUCCEEDED(hr) && m_pXAudio2 && m_pBuffer)
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


		}
		return *this;
	}
	//���[�u���(nullptr�����˂�)
	inline CSourceVoice& operator =(CSourceVoice&& right) noexcept
	{
		if (*this != right)
		{
			if (m_started)Stop();
			if (m_pSourceVoice)m_pSourceVoice->DestroyVoice(), m_pSourceVoice = nullptr;//�|�C���^��t���ւ��邽�߁CDestroyVoice
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

			right.m_started = false;//������̉������~���邱�Ƃ�h��
			right.m_pSourceVoice = nullptr;//�������SourceVoice��DestroyVoice���邱�Ƃ�h��
		}
		return *this;
	}
	//��r���Z�q
	bool operator ==(CSourceVoice& right) noexcept
	{
		return this->m_pSourceVoice == right.m_pSourceVoice;
	}
	//��r���Z�q
	bool operator !=(CSourceVoice& right) noexcept
	{
		return !(*this == right);
	}
	//�Đ������ǂ���
	_NODISCARD inline bool isRunning(void)const noexcept
	{
		return Xaudio2CSourvoiceInterface::isRunning();
	}
	//true�̎��ɊJ�n����Ȃǁi�O�����玩�R�Ɏg���j
	bool flag = false;
private:
	static constexpr size_t bitPerByte = std::numeric_limits<BYTE>::digits;
	//point����n�܂�T���v���̊e�`�����l���̕��ς�Ԃ��B
	_NODISCARD inline float getSampleAvg(size_t index) const noexcept
	{
		if (m_pWfx)
		{
			size_t uiBytesPerSample = m_pWfx->wBitsPerSample / bitPerByte;//1�T���v���̃T�C�Y�i�o�C�g�P�ʁj
			size_t uiSamplesPlayedPerChannel = std::max<size_t>(index - 1, 0) / m_pWfx->nChannels;//�T���v���̐����`�����l�����Ŋ���������-1
			size_t point = uiSamplesPlayedPerChannel * m_pWfx->nChannels * uiBytesPerSample;//�T���v���̈�ԏ��߂̗v�f�̈ʒu
			point %= m_audioData.size();//�T���v���̈�ԏ��߂̗v�f�̈ʒu�i2���ڈȍ~�̉\�������邽�߁C�]������߂�j
			if (m_pWfx->wBitsPerSample == 8)
			{
				std::vector<INT8> Samples(m_pWfx->nChannels, 0);
				for (size_t i = 0; i < m_pWfx->nChannels; ++i)
				{
					Samples.at(i) = std::bit_cast<INT8>(m_audioData.at(point + i));
				}
				int avg = std::accumulate(Samples.begin(), Samples.end(), 0) / static_cast<int>(m_pWfx->nChannels);//�e�`�����l���̕��ϒl
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
						temp += m_audioData.at(point + uiBytesPerSample * i + uiBytesPerSample - 1 - j);//���g���G���f�B�A���̂��߁C�ォ��ǂݍ���
					}
					Samples.at(i) = std::bit_cast<INT16>(temp);
				}
				int avg = std::accumulate(Samples.begin(), Samples.end(), 0) / static_cast<int>(m_pWfx->nChannels);//�e�`�����l���̕��ϒl
				return std::abs(static_cast<float>(avg) / static_cast<float>(std::numeric_limits<INT16>::max()));
			}
			else return 1.0f;
		}
		else return 0.0f;
	}
	//�������o�b�t�@�ɓǂݍ��݁C�\�[�X�{�C�X�����B
	HRESULT CreateSourceVoice(
		const std::filesystem::path& name,//�����f�[�^�̃t�@�C����
		FILE* fp = nullptr//���������O�t�@�C���ւ̃|�C���^
	)
	{
		std::filesystem::path name_normal(name.lexically_normal());
		bool mfStarted = false;//���f�B�A�t�@���f�[�V�����v���b�g�t�H�[����������������TRUE�ɂ���B�i�ʏ�͍Ō�܂�FALSE�j
		winrt::com_ptr<IMFSourceReader>pReader;//�\�[�X���[�_�[�\����
		winrt::com_ptr<IMFMediaType>pMediaType;//�ǂݍ��݃t�@�C���̃^�C�v
		winrt::com_ptr<IMFMediaType>pOutputMediaType;//�Đ��f�[�^�̃^�C�v
		HRESULT hr;//COM�֐��̖߂�l
		//https://github.com/microsoft/Windows-universal-samples/blob/master/Samples/SpatialSound/cpp/AudioFileReader.cpp
		hr = MFCreateSourceReaderFromURL(name_normal.c_str(), nullptr, pReader.put());//�t�@�C�����J��
		if (FAILED(hr))//���s������MFStartup�����s���Ă�����Ȃ����B
		{
			hr = MFStartup(MF_VERSION);// ���f�B�A�t�@���f�[�V�����v���b�g�t�H�[���̏�����
			mfStarted = SUCCEEDED(hr);//�������o������TRUE�ɂ���B
			hr = MFCreateSourceReaderFromURL(name_normal.c_str(), nullptr, pReader.put());//�t�@�C�����J��
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
				auto currentDataSize = m_audioData.size();
				m_audioData.resize(currentDataSize + dwBufSize);
				CopyMemory(m_audioData.data() + currentDataSize, pbData, dwBufSize);
//				std::copy(pbData, pbData + dwBufSize, std::back_inserter(m_audioData));
				hr = pBuf->Unlock();//�������̉���idelete���Ă͂����Ȃ��j
			}
		}
		m_audioData.shrink_to_fit();
		// �\�[�X�{�C�X�̍쐬
		if (SUCCEEDED(hr))hr = m_pXAudio2->CreateSourceVoice(&m_pSourceVoice, m_pWfx.get(), m_Flags, m_MaxFrequencyRatio, m_pCallback, m_pSendList, m_pEffectChain);
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
			if (fp)fwprintf_s(fp, L"�\�[�X�{�C�X�̍쐬���s\n�G���[:%#X\n", hr);
			if (fp)fwprintf_s(fp, L"�t�@�C�����F%s\n", name_normal.c_str());
			//						fprintf_s(fp1, L"�A�h���X�F%p\r\n", motornoise[i]);
		}
		else//���̃��[�^�[���̃\�[�X�{�C�X���쐬�ł����B
		{
			if (fp)fwprintf_s(fp, L"�\�[�X�{�C�X�̍쐬����\n");
			if (fp)fwprintf_s(fp, L"�t�@�C�����F%s\n", name_normal.c_str());
			//						fprintf_s(fp1, L"�A�h���X�F%p\r\n", motornoise[i]);
		}
		return hr;
	}
};




#endif // !CSOURCEVOICE_INCLUDED