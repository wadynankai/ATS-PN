#ifndef CAUDIOFILEINPUTNODE_INCLUDED
#define CAUDIOFILEINPUTNODE_INCLUDED
#include <filesystem>
#include <chrono>
#include <thread>
#ifndef _WIN32_WINNT
#include <winsdkver.h>
#define _WIN32_WINNT//�ŐV�o�[�W������Windows
#endif //_WIN32_WINNT
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <sdkddkver.h>
#include <optional>
#include <fstream>
#include <winrt\windows.foundation.h>
#include <winrt\windows.media.audio.h>
#include <winrt\windows.storage.h>


class CAudioFileInputNode
{
public:
	//��̃R���X�g���N�^
	CAudioFileInputNode(nullptr_t p = nullptr)noexcept :m_node(nullptr), m_started(false), flag(false) {}
	//�R�s�[�R���X�g���N�^
	CAudioFileInputNode(const CAudioFileInputNode& right)
		:m_node(right.m_node),
		m_started(false), flag(false) {}
	//���[�u�R���X�g���N�^
	CAudioFileInputNode(CAudioFileInputNode&& right)noexcept
		:m_node(std::move(right.m_node)), m_started(std::move(right.m_started)), flag(std::move(right.flag)) {}
	//�R���X�g���N�^
	CAudioFileInputNode(
		const winrt::Windows::Media::Audio::AudioGraph& graph, //AudioGraph�I�u�W�F�N�g�ւ̎Q��
		const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode,//OutputNode�ւ̎Q��
		const std::filesystem::path& name,//�����t�@�C���̃t�@�C����
		winrt::Windows::Foundation::IReference<int32_t> LoopCount = nullptr//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�Cnullptr�͖������[�v
	)
		: CAudioFileInputNode(graph, outputNode, nullptr, nullptr, name, LoopCount) {}
	//�R���X�g���N�^
	CAudioFileInputNode(
		const winrt::Windows::Media::Audio::AudioGraph& graph, //AudioGraph�I�u�W�F�N�g�ւ̎Q��
		const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode,//OutputNode�ւ̎Q��
		winrt::hresult* pHr,//�G���[�ԍ�
		const std::filesystem::path& name,//�����t�@�C���̃t�@�C����
		winrt::Windows::Foundation::IReference<int32_t> LoopCount = nullptr//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�Cnullptr�͖������[�v
	)
		: CAudioFileInputNode(graph, outputNode, pHr, nullptr, name, LoopCount) {}
	//�R���X�g���N�^
	CAudioFileInputNode(
		const winrt::Windows::Media::Audio::AudioGraph& graph, //AudioGraph�I�u�W�F�N�g�ւ̎Q��
		const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode,//OutputNode�ւ̎Q��
		std::wfilebuf* pBuf,//���������O�t�@�C���ւ̃|�C���^
		const std::filesystem::path& name,//�����t�@�C���̃t�@�C����
		winrt::Windows::Foundation::IReference<int32_t> LoopCount = nullptr//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�Cnullptr�͖������[�v
	)
		: CAudioFileInputNode(graph, outputNode, nullptr, pBuf, name, LoopCount) {}
	//�R���X�g���N�^
	CAudioFileInputNode(
		const winrt::Windows::Media::Audio::AudioGraph& graph, //AudioGraph�I�u�W�F�N�g�ւ̎Q��
		const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode,//OutputNode�ւ̎Q��
		winrt::hresult* pHr,//�G���[�ԍ�
		std::wfilebuf* pBuf,//���������O�t�@�C���ւ̃|�C���^
		const std::filesystem::path& name,//�����t�@�C���̃t�@�C����
		winrt::Windows::Foundation::IReference<int32_t> LoopCount = nullptr//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�Cnullptr�͖������[�v
	)
		:m_node(nullptr), m_started(false), flag(false)
	{
		winrt::hresult hr = CreateAudioFileInputNode(name, graph, outputNode, LoopCount, pBuf);
		if (pHr)*pHr = hr;
	}

	//�f�X�g���N�^
	~CAudioFileInputNode() = default;

	//�R�s�[���
	inline CAudioFileInputNode& operator=(const CAudioFileInputNode& right) = default;
	//���[�u���
	inline CAudioFileInputNode& operator=(CAudioFileInputNode&& right)noexcept
	{
		m_node = std::move(right.m_node);
		m_started = std::move(right.m_started);
		flag = std::move(right.flag);
		return *this;
	}
	//nullptr�̑��
	inline CAudioFileInputNode& operator=(nullptr_t p)noexcept
	{
		m_node = nullptr;
		m_started = false;
		flag = false;
		return *this;
	}
	//��r���Z�q
	bool operator==(CAudioFileInputNode& right) noexcept
	{
		return m_node == right.m_node;
	}
	//��r���Z�q
	bool operator!=(CAudioFileInputNode& right) noexcept
	{
		return !(*this == right);
	}
	//�t�@�C���C���v�b�g�m�[�h���L�����ǂ�����Ԃ�
	inline explicit operator bool(void) const noexcept
	{
		return static_cast<bool>(m_node);
	}
	//�Đ�
	inline void Start(void) noexcept
	{
		if (m_node)
		{
			if (m_node.LoopCount() == nullptr)
			{
				m_node.Start();
				m_started = true;
			}
			else
			{
				if (isRunning())//�Đ��r���̏ꍇ
				{
					m_node.Stop();
					m_started = false;
				}
				if (!m_started)//�����C��~���Ă����ꍇ�C�Đ�����B
				{
					m_node.Reset();
					m_node.Start();
					m_started = true;
				}
			}
		}
	}
	//��~
	inline void Stop(void) noexcept
	{
		if (m_node)
		{
			if (m_node.LoopCount() == nullptr)
			{
				m_node.Stop();
				m_started = false;
			}
			else
			{
				m_node.Stop();
				m_started = false;
				m_node.Reset();
			}
		}
	}

	//���ʂ�0����1�̊ԂŐݒ�
	inline void OutgoingGain_0_1(
		const double vol//���ʁi1�ŃI���W�i���̉��ʁj
	) const noexcept
	{
		if (m_node)m_node.OutgoingGain(std::clamp(vol, 0.0, 1.0));
	}
	//���ʂ�ݒ�
	inline void OutgoingGain(
		const double vol//���ʁi1�ŃI���W�i���̉��ʁj
	) const noexcept
	{
		if (m_node)m_node.OutgoingGain(vol);
	}	
	//�Đ����x��ݒ�
	inline void PlaybackSpeedFactor(
		const double speed//�Đ����x�i1�ŃI���W�i���̑��x�j
	) const noexcept
	{
		if (m_node)m_node.PlaybackSpeedFactor(speed);
	}
	//�J�n�ʒu��ݒ�
	inline void StartTime(
		const winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::TimeSpan> time
	) const noexcept
	{
		if (m_node)m_node.StartTime(time);
	}	
	//�J�n�ʒu���擾
	_NODISCARD inline winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::TimeSpan> StartTime(void) const noexcept
	{
		if (m_node)return m_node.StartTime();
		else return nullptr;
	}
	//�I���ʒu��ݒ�
	inline void EndTime(
		const winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::TimeSpan> time
	) const noexcept
	{
		if (m_node)m_node.EndTime(time);
	}
	//�I���ʒu���擾
	_NODISCARD inline winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::TimeSpan> EndTime(void) const noexcept
	{
		if (m_node)return m_node.EndTime();
		else return nullptr;
	}
	//�������擾
	_NODISCARD inline winrt::Windows::Foundation::TimeSpan Duration(void) const noexcept
	{
		if (m_node)return m_node.Duration();
		else return winrt::Windows::Foundation::TimeSpan{ 0 };
	}
	//�J��Ԃ��񐔂�ݒ�
	inline void LoopCount(
		const winrt::Windows::Foundation::IReference<int32_t> count
	) const noexcept
	{
		if (m_node)m_node.LoopCount(count);
	}
	//�J��Ԃ��񐔂��擾
	_NODISCARD inline winrt::Windows::Foundation::IReference<int32_t> LoopCount(void) const noexcept
	{
		if (m_node)return m_node.LoopCount();
		else return nullptr;
	}
	//�Đ��ʒu�̎擾
	_NODISCARD inline winrt::Windows::Foundation::TimeSpan Position(void) const noexcept
	{
		if (m_node)return m_node.Position();
		else return winrt::Windows::Foundation::TimeSpan{ 0 };
	}
	//�Đ������ǂ���
	_NODISCARD inline bool isRunning(void) noexcept
	{
		if (m_node)
		{
			if (m_node.LoopCount() != nullptr && m_started)
			{
				auto endTime = m_node.EndTime();
				winrt::Windows::Foundation::TimeSpan tsEndTime;
				if (endTime != nullptr)tsEndTime = endTime.Value();
				else tsEndTime = m_node.Duration();
				auto msEndTime = std::chrono::duration_cast<std::chrono::milliseconds>(tsEndTime);
				if (std::chrono::duration_cast<std::chrono::milliseconds>(m_node.Position()) >= msEndTime)
				{
					m_started = false;
				}
			}
			return m_started;
		}
		else return false;
	}
	//���o��
	inline void Reset()const noexcept
	{
		if (m_node)m_node.Reset();
	}
	//���o��
	inline void Cue(void) const noexcept
	{
		if (m_node)
		{
			m_node.Reset();
			if(m_started)m_node.Start();
		}
	}
	//�ꎞ��~�im_started��m_buffer��ύX�����ɒ�~����B�j
	void pause(void) const noexcept
	{
		if (m_node)m_node.Stop();
	}
	//�ꎞ��~�̉����im_started��m_buffer��ύX�����Đ�����B�j
	void pauseDefeat(void)const noexcept
	{
		if(m_node)m_node.Start();
	}
	//�I���i�����j
	inline void Close()
	{
		if (m_node)
		{
			m_node.Close();
		}
		m_started = false;
		flag = false;
	}
	//true�̎��ɊJ�n����Ȃǁi�O�����玩�R�Ɏg���j
	bool flag;


private:
	winrt::Windows::Media::Audio::AudioFileInputNode m_node;
	bool m_started;
	winrt::hresult CreateAudioFileInputNode(
		const std::filesystem::path& name,//�����f�[�^�̃t�@�C����
		const winrt::Windows::Media::Audio::AudioGraph& graph,
		const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode,
		const winrt::Windows::Foundation::IReference<int32_t>& LoopCount = nullptr,
		std::wfilebuf* pBuf = nullptr//���������O�t�@�C���ւ̃|�C���^
	)
	{

		std::filesystem::path name_normal(name.lexically_normal());
		winrt::Windows::Storage::StorageFile storageFile = nullptr;
		winrt::Windows::Media::Audio::CreateAudioFileInputNodeResult fResult = nullptr;
		if (!std::filesystem::exists(name_normal))//�t�@�C�������݂��Ȃ��ꍇ
		{
			if (pBuf)
			{
				if (pBuf->is_open())
				{
					std::basic_ostream ostr(pBuf);
					ostr << L"�t�@�C���F" << name_normal << L"�͑��݂��܂���B�X�L�b�v���܂��B" << std::endl;
				}
			}
		}
		else
		{
			std::thread th{ [&name,&graph,&outputNode,pBuf,&LoopCount,this,&name_normal,&storageFile,&fResult]() {
			try
			{
				storageFile = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(name_normal.wstring()).get();
				fResult = graph.CreateFileInputNodeAsync(storageFile).get();
			}
			catch (const std::exception& ex)
			{
				if (pBuf)
				{
					if (pBuf->is_open())
					{
						std::basic_ostream ostr(pBuf);
						ostr << L"DeviceOutputNode�쐬���s" << std::endl;
						ostr << L"�G���[: " << std::hex << fResult.ExtendedError().value << std::endl;
						ostr << ex.what() << std::endl;
					}
				}
			}
			if (fResult.Status() == winrt::Windows::Media::Audio::AudioFileNodeCreationStatus::Success)//�쐬����
			{
				fResult.FileInputNode().Stop();
				fResult.FileInputNode().AddOutgoingConnection(outputNode);
				fResult.FileInputNode().LoopCount(LoopCount);
				if (pBuf)
				{
					if (pBuf->is_open())
					{
						std::basic_ostream ostr(pBuf);
						ostr << L"AudioFileInputNode�̍쐬����" << std::endl;
						ostr << L"�t�@�C�����F" << std::endl;
						ostr << name_normal.c_str() << std::endl;
					}
				}
				m_node = fResult.FileInputNode();
				if (LoopCount == nullptr)//���[�v�Đ���
				{
					this->Start();
					this->OutgoingGain(0.0);
				}
			}
			} };
			if (th.joinable())th.join();
		}
		if (fResult) return fResult.ExtendedError();
		else return winrt::hresult(0x80004005L);//E_FAIL
	}

};


#endif // !CAUDIOFILEINPUTNODE_INCLUDED