#ifndef CAUDIOFILEINPUTNODE_INCLUDED
#define CAUDIOFILEINPUTNODE_INCLUDED
#include <filesystem>
#include <chrono>
#include <thread>
#ifndef _WIN32_WINNT
#include <winsdkver.h>
#define _WIN32_WINNT//最新バージョンのWindows
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
	//空のコンストラクタ
	CAudioFileInputNode(nullptr_t p = nullptr)noexcept :m_node(nullptr), m_started(false), flag(false) {}
	//コピーコンストラクタ
	CAudioFileInputNode(const CAudioFileInputNode& right)
		:m_node(right.m_node),
		m_started(false), flag(false) {}
	//ムーブコンストラクタ
	CAudioFileInputNode(CAudioFileInputNode&& right)noexcept
		:m_node(std::move(right.m_node)), m_started(std::move(right.m_started)), flag(std::move(right.flag)) {}
	//コンストラクタ
	CAudioFileInputNode(
		const winrt::Windows::Media::Audio::AudioGraph& graph, //AudioGraphオブジェクトへの参照
		const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode,//OutputNodeへの参照
		const std::filesystem::path& name,//音声ファイルのファイル名
		winrt::Windows::Foundation::IReference<int32_t> LoopCount = nullptr//ループカウント（1回の場合は0，2回の場合は1，…，nullptrは無限ループ
	)
		: CAudioFileInputNode(graph, outputNode, nullptr, nullptr, name, LoopCount) {}
	//コンストラクタ
	CAudioFileInputNode(
		const winrt::Windows::Media::Audio::AudioGraph& graph, //AudioGraphオブジェクトへの参照
		const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode,//OutputNodeへの参照
		winrt::hresult* pHr,//エラー番号
		const std::filesystem::path& name,//音声ファイルのファイル名
		winrt::Windows::Foundation::IReference<int32_t> LoopCount = nullptr//ループカウント（1回の場合は0，2回の場合は1，…，nullptrは無限ループ
	)
		: CAudioFileInputNode(graph, outputNode, pHr, nullptr, name, LoopCount) {}
	//コンストラクタ
	CAudioFileInputNode(
		const winrt::Windows::Media::Audio::AudioGraph& graph, //AudioGraphオブジェクトへの参照
		const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode,//OutputNodeへの参照
		std::wfilebuf* pBuf,//初期化ログファイルへのポインタ
		const std::filesystem::path& name,//音声ファイルのファイル名
		winrt::Windows::Foundation::IReference<int32_t> LoopCount = nullptr//ループカウント（1回の場合は0，2回の場合は1，…，nullptrは無限ループ
	)
		: CAudioFileInputNode(graph, outputNode, nullptr, pBuf, name, LoopCount) {}
	//コンストラクタ
	CAudioFileInputNode(
		const winrt::Windows::Media::Audio::AudioGraph& graph, //AudioGraphオブジェクトへの参照
		const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode,//OutputNodeへの参照
		winrt::hresult* pHr,//エラー番号
		std::wfilebuf* pBuf,//初期化ログファイルへのポインタ
		const std::filesystem::path& name,//音声ファイルのファイル名
		winrt::Windows::Foundation::IReference<int32_t> LoopCount = nullptr//ループカウント（1回の場合は0，2回の場合は1，…，nullptrは無限ループ
	)
		:m_node(nullptr), m_started(false), flag(false)
	{
		winrt::hresult hr = CreateAudioFileInputNode(name, graph, outputNode, LoopCount, pBuf);
		if (pHr)*pHr = hr;
	}

	//デストラクタ
	~CAudioFileInputNode() = default;

	//コピー代入
	inline CAudioFileInputNode& operator=(const CAudioFileInputNode& right) = default;
	//ムーブ代入
	inline CAudioFileInputNode& operator=(CAudioFileInputNode&& right)noexcept
	{
		m_node = std::move(right.m_node);
		m_started = std::move(right.m_started);
		flag = std::move(right.flag);
		return *this;
	}
	//nullptrの代入
	inline CAudioFileInputNode& operator=(nullptr_t p)noexcept
	{
		m_node = nullptr;
		m_started = false;
		flag = false;
		return *this;
	}
	//比較演算子
	bool operator==(CAudioFileInputNode& right) noexcept
	{
		return m_node == right.m_node;
	}
	//比較演算子
	bool operator!=(CAudioFileInputNode& right) noexcept
	{
		return !(*this == right);
	}
	//ファイルインプットノードが有効かどうかを返す
	inline explicit operator bool(void) const noexcept
	{
		return static_cast<bool>(m_node);
	}
	//再生
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
				if (isRunning())//再生途中の場合
				{
					m_node.Stop();
					m_started = false;
				}
				if (!m_started)//もし，停止していた場合，再生する。
				{
					m_node.Reset();
					m_node.Start();
					m_started = true;
				}
			}
		}
	}
	//停止
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

	//音量を0から1の間で設定
	inline void OutgoingGain_0_1(
		const double vol//音量（1でオリジナルの音量）
	) const noexcept
	{
		if (m_node)m_node.OutgoingGain(std::clamp(vol, 0.0, 1.0));
	}
	//音量を設定
	inline void OutgoingGain(
		const double vol//音量（1でオリジナルの音量）
	) const noexcept
	{
		if (m_node)m_node.OutgoingGain(vol);
	}	
	//再生速度を設定
	inline void PlaybackSpeedFactor(
		const double speed//再生速度（1でオリジナルの速度）
	) const noexcept
	{
		if (m_node)m_node.PlaybackSpeedFactor(speed);
	}
	//開始位置を設定
	inline void StartTime(
		const winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::TimeSpan> time
	) const noexcept
	{
		if (m_node)m_node.StartTime(time);
	}	
	//開始位置を取得
	_NODISCARD inline winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::TimeSpan> StartTime(void) const noexcept
	{
		if (m_node)return m_node.StartTime();
		else return nullptr;
	}
	//終了位置を設定
	inline void EndTime(
		const winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::TimeSpan> time
	) const noexcept
	{
		if (m_node)m_node.EndTime(time);
	}
	//終了位置を取得
	_NODISCARD inline winrt::Windows::Foundation::IReference<winrt::Windows::Foundation::TimeSpan> EndTime(void) const noexcept
	{
		if (m_node)return m_node.EndTime();
		else return nullptr;
	}
	//長さを取得
	_NODISCARD inline winrt::Windows::Foundation::TimeSpan Duration(void) const noexcept
	{
		if (m_node)return m_node.Duration();
		else return winrt::Windows::Foundation::TimeSpan{ 0 };
	}
	//繰り返し回数を設定
	inline void LoopCount(
		const winrt::Windows::Foundation::IReference<int32_t> count
	) const noexcept
	{
		if (m_node)m_node.LoopCount(count);
	}
	//繰り返し回数を取得
	_NODISCARD inline winrt::Windows::Foundation::IReference<int32_t> LoopCount(void) const noexcept
	{
		if (m_node)return m_node.LoopCount();
		else return nullptr;
	}
	//再生位置の取得
	_NODISCARD inline winrt::Windows::Foundation::TimeSpan Position(void) const noexcept
	{
		if (m_node)return m_node.Position();
		else return winrt::Windows::Foundation::TimeSpan{ 0 };
	}
	//再生中かどうか
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
	//頭出し
	inline void Reset()const noexcept
	{
		if (m_node)m_node.Reset();
	}
	//頭出し
	inline void Cue(void) const noexcept
	{
		if (m_node)
		{
			m_node.Reset();
			if(m_started)m_node.Start();
		}
	}
	//一時停止（m_startedとm_bufferを変更せずに停止する。）
	void pause(void) const noexcept
	{
		if (m_node)m_node.Stop();
	}
	//一時停止の解除（m_startedとm_bufferを変更せず再生する。）
	void pauseDefeat(void)const noexcept
	{
		if(m_node)m_node.Start();
	}
	//終了（消去）
	inline void Close()
	{
		if (m_node)
		{
			m_node.Close();
		}
		m_started = false;
		flag = false;
	}
	//trueの時に開始するなど（外部から自由に使う）
	bool flag;


private:
	winrt::Windows::Media::Audio::AudioFileInputNode m_node;
	bool m_started;
	winrt::hresult CreateAudioFileInputNode(
		const std::filesystem::path& name,//音声データのファイル名
		const winrt::Windows::Media::Audio::AudioGraph& graph,
		const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode,
		const winrt::Windows::Foundation::IReference<int32_t>& LoopCount = nullptr,
		std::wfilebuf* pBuf = nullptr//初期化ログファイルへのポインタ
	)
	{

		std::filesystem::path name_normal(name.lexically_normal());
		winrt::Windows::Storage::StorageFile storageFile = nullptr;
		winrt::Windows::Media::Audio::CreateAudioFileInputNodeResult fResult = nullptr;
		if (!std::filesystem::exists(name_normal))//ファイルが存在しない場合
		{
			if (pBuf)
			{
				if (pBuf->is_open())
				{
					std::basic_ostream ostr(pBuf);
					ostr << L"ファイル：" << name_normal << L"は存在しません。スキップします。" << std::endl;
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
						ostr << L"DeviceOutputNode作成失敗" << std::endl;
						ostr << L"エラー: " << std::hex << fResult.ExtendedError().value << std::endl;
						ostr << ex.what() << std::endl;
					}
				}
			}
			if (fResult.Status() == winrt::Windows::Media::Audio::AudioFileNodeCreationStatus::Success)//作成成功
			{
				fResult.FileInputNode().Stop();
				fResult.FileInputNode().AddOutgoingConnection(outputNode);
				fResult.FileInputNode().LoopCount(LoopCount);
				if (pBuf)
				{
					if (pBuf->is_open())
					{
						std::basic_ostream ostr(pBuf);
						ostr << L"AudioFileInputNodeの作成成功" << std::endl;
						ostr << L"ファイル名：" << std::endl;
						ostr << name_normal.c_str() << std::endl;
					}
				}
				m_node = fResult.FileInputNode();
				if (LoopCount == nullptr)//ループ再生時
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