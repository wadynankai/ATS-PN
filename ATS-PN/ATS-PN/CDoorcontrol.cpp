#include "CDoorcontrol.h"

inline void CDoorcontrol::loadconfig(void)
{
	std::filesystem::path DoorClsL_name;
	std::filesystem::path DoorClsR_name;
	std::filesystem::path DoorOpnL_name;
	std::filesystem::path DoorOpnR_name;
	std::wifstream Config(m_module_dir / L"DoorSoundConfig.txt");
	if (!Config.fail())
	{
		Config.imbue(std::locale(".UTF-8"));
		while (!Config.eof())
		{
			std::wstring loadline;
			std::getline(Config, loadline);
			cleanUpBveStr(loadline, Config.getloc());
			std::wstring left, right;
			if (splitSymbol(L'=', loadline, left, right, Config.getloc()) != std::wstring::npos)
			{
				if (icasecmp(left, L"Close Left", Config.getloc()))DoorClsL_name = m_module_dir / right;
				else if (icasecmp(left, L"Close Right", Config.getloc()))DoorClsR_name = m_module_dir / right;
				else if (icasecmp(left, L"Open Left", Config.getloc()))DoorOpnL_name = m_module_dir / right;
				else if (icasecmp(left, L"Open Right", Config.getloc()))DoorOpnR_name = m_module_dir / right;
			}
		}
	}
	Config.close();

	m_DoorClsL = CAudioFileInputNode(m_graph, m_outputNode, DoorClsL_name, 0);
	m_DoorClsR = CAudioFileInputNode(m_graph, m_outputNode, DoorClsR_name, 0);
	m_DoorOpnL = CAudioFileInputNode(m_graph, m_outputNode, DoorOpnL_name, 0);
	m_DoorOpnR = CAudioFileInputNode(m_graph, m_outputNode, DoorOpnR_name, 0);
}

CDoorcontrol::CDoorcontrol(const std::filesystem::path& moduleDir,
	const winrt::Windows::Media::Audio::AudioGraph& graph, const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode) :
	m_graph(graph),
	m_outputNode(outputNode),
	m_module_dir(moduleDir),
	m_tableFileName(m_module_dir / L"doorConfig.csv"),
	doorUmi(0), doorYama(0), m_trainNo(0),
	m_DoorClsL(nullptr), m_DoorClsR(nullptr), m_DoorOpnL(nullptr), m_DoorOpnR(nullptr),
	m_nambaF(false), m_nambaTrack(0),
	m_OpenTime(0), m_OpenTime_pre(0),
	m_sanoF(false), m_sanoTrack(0)
{
	if (graph == nullptr)
	{
		winrt::Windows::Media::Audio::AudioGraphSettings settings{ winrt::Windows::Media::Render::AudioRenderCategory::GameMedia };
		settings.MaxPlaybackSpeedFactor(1.0);
		winrt::Windows::Media::Audio::CreateAudioGraphResult result = winrt::Windows::Media::Audio::AudioGraph::CreateAsync(settings).get();
		if (result.Status() == winrt::Windows::Media::Audio::AudioGraphCreationStatus::Success)
		{
			m_graph = result.Graph();

			winrt::Windows::Media::Audio::CreateAudioDeviceOutputNodeResult oResult = m_graph.CreateDeviceOutputNodeAsync().get();
			if (oResult.Status() == winrt::Windows::Media::Audio::AudioDeviceNodeCreationStatus::Success)
			{
				m_outputNode = oResult.DeviceOutputNode();
			}
		}
		else if (outputNode == nullptr)
		{
			winrt::Windows::Media::Audio::CreateAudioDeviceOutputNodeResult oResult = m_graph.CreateDeviceOutputNodeAsync().get();
			if (oResult.Status() == winrt::Windows::Media::Audio::AudioDeviceNodeCreationStatus::Success)
			{
				m_outputNode = oResult.DeviceOutputNode();
			}
		}
		m_graph.Start();
	}
	loadconfig();
}

CDoorcontrol::~CDoorcontrol()
{
	std::thread th{ [this]()
	{
		this->m_graph.Stop();
		this->m_DoorClsL = nullptr;
		this->m_DoorClsR = nullptr;
		this->m_DoorOpnL = nullptr;
		this->m_DoorOpnR = nullptr;
		this->m_outputNode = nullptr;
		this->m_graph = nullptr;
	} };
	if(th.joinable())th.detach();
}

void CDoorcontrol::setTrainNo(const int no)
{
	m_trainNo = no;
	std::wifstream table(m_tableFileName);
	table.imbue(std::locale(".UTF-8"));
	if (!table.fail())
	{
		while (!table.eof())
		{
			std::wstring loadline;
			std::getline(table, loadline);
			std::vector<std::wstring> columun;
			cleanUpBveStr(loadline, table.getloc());
			eraseSpace(loadline, table.getloc());
			if (!loadline.empty())
			{
				splitCsvColumn(loadline, columun, table.getloc());
				if (std::stoi(columun.at(0)) == m_trainNo&& columun.size()>=3)
				{
					if (!columun.at(1).empty() && (std::all_of(columun.at(1).cbegin(), columun.at(1).cend(), isdigit)))m_nambaTrack = std::stoi(columun.at(1));
					if (!columun.at(2).empty() && (std::all_of(columun.at(2).cbegin(), columun.at(2).cend(), isdigit)))m_sanoTrack = std::stoi(columun.at(2));
				}
			}
		}
	}
	table.close();
}




