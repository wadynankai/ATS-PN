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
	//�R���X�g���N�^

	CSourceVoice(
		IXAudio2* Xau2,//IXAudio2�C���^�[�t�F�[�X�ւ̃|�C���^ 
		const std::wstring& name,//�����t�@�C���̃t�@�C����
		UINT32 LoopCount = XAUDIO2_LOOP_INFINITE//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�C�ȗ������ꍇ�͖������[�v
	);

	CSourceVoice(
		IXAudio2* Xau2,//IXAudio2�C���^�[�t�F�[�X�ւ̃|�C���^ 
		FILE* fp,//���������O�t�@�C���ւ̃|�C���^
		const std::wstring& name,//�����t�@�C���̃t�@�C����
		UINT32 LoopCount = XAUDIO2_LOOP_INFINITE//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�C�ȗ������ꍇ�͖������[�v
	);
	~CSourceVoice();
	//�R���X�g���N�^�Ɠ�������������B
	void Setparam(
		IXAudio2* Xau2,//IXAudio2�C���^�[�t�F�[�X�ւ̃|�C���^ 
		const std::wstring& name,//�����t�@�C���̃t�@�C����
		UINT32 LoopCount = XAUDIO2_LOOP_INFINITE//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�ȗ������ꍇ�͖������[�v
	);

	void Setparam(
		IXAudio2* Xau2,//IXAudio2�C���^�[�t�F�[�X�ւ̃|�C���^ 
		FILE* fp,//���������O�t�@�C���ւ̃|�C���^
		const std::wstring& name,//�����t�@�C���̃t�@�C����
		UINT32 LoopCount = XAUDIO2_LOOP_INFINITE//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�ȗ������ꍇ�͖������[�v
	);

	//�Đ�
	HRESULT Start(
		UINT32 OperationSet = XAUDIO2_COMMIT_NOW//�I�y���[�V�����Z�b�g�i�ȗ��j
	);
	//��~
	HRESULT Stop(
		UINT32 OperationSet = XAUDIO2_COMMIT_NOW//�I�y���[�V�����Z�b�g�i�ȗ��j
	);
	//�s�b�`��ݒ�
	HRESULT SetFrequencyRatio(
		float ratio,//�s�b�`�i1�ŃI���W�i���̃X�s�[�h�j
		UINT32 OperationSet = XAUDIO2_COMMIT_NOW//�I�y���[�V�����Z�b�g�i�ȗ��j
	);
	//���ʂ�ݒ�
	HRESULT SetVolume(
		float vol,//���ʁi1�ŃI���W�i���̉��ʁj
		UINT32 OperationSet = XAUDIO2_COMMIT_NOW//�I�y���[�V�����Z�b�g�i�ȗ��j
	);
	IXAudio2SourceVoice* pSourceVoice() const;//�\�[�X�{�C�X�̊֐��𒼐ڎg�������Ƃ��Ɏg��
	bool isRunning(void) const;//�Đ������ǂ���
	bool flag = false;//true�̎��ɊJ�n����Ȃǁi�O�����玩�R�Ɏg���j
	explicit operator bool() const noexcept;
private:
	IXAudio2* m_XAudio2 = nullptr;//IXAudio2�ւ̃|�C���^
	IXAudio2SourceVoice* m_pSourceVoice = nullptr;//�\�[�X�{�C�X�ւ̃|�C���^
	HRESULT StartInf(UINT32 OperationSet);//�������[�v���̍Đ�
	HRESULT StartBound(UINT32 OperationSet);//�Đ��񐔗L�����̍Đ�
	HRESULT StopInf(UINT32 OperationSet);//�������[�v���̒�~
	HRESULT StopBound(UINT32 OperationSet);//�Đ��񐔗L�����̒�~
	LPWSTR m_szFilename = nullptr;//�����t�@�C����
	//�������o�b�t�@�ɓǂݍ��݁C�\�[�X�{�C�X�����B
	HRESULT CreateSourceVoice(
		FILE* fp = nullptr//���������O�t�@�C���ւ̃|�C���^
	);
	std::vector<BYTE> m_audioData;//�����f�[�^��ۑ�����̈�
	UINT32 m_LoopCount;///���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�ȗ������ꍇ�͖������[�v
	XAUDIO2_BUFFER m_buffer;//XAUDIO2_BUFFER�\����
	bool m_started;//start���Ă���stop����܂�true
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
		if (isRunning())//�Đ��r���̏ꍇ
		{
			if (FAILED(hr = StopBound(XAUDIO2_COMMIT_NOW)))return hr;//���߂���Đ����Ȃ������߂ɁC�~�߂ăo�b�t�@����������B(OperationSet���g�킸�C�����ɒ�~�j
		}
		if (FAILED(hr = m_pSourceVoice->SubmitSourceBuffer(&m_buffer)))return hr;//�o�b�t�@���Ƀf�[�^������B
		if (!m_started)//�����C��~���Ă����ꍇ�C�Đ�����B
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