#include "CAutoAnnounce.h"
CAutoAnnounce::CAutoAnnounce(const std::filesystem::path& moduleDir, int& DelT,
	const winrt::Windows::Media::Audio::AudioGraph& graph, const winrt::Windows::Media::Audio::AudioDeviceOutputNode& outputNode):
	m_DelT(DelT),//1フレームの時間
	m_graph(graph),
	m_outputNode(outputNode),
	m_Announce1(nullptr),
	m_Announce2(nullptr),
	m_module_dir(moduleDir),
	m_table_dir(moduleDir / L"announce\\")
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	if (graph == nullptr)
	{
		std::thread th{ [&]()
		{
			try
			{
				winrt::Windows::Media::Audio::AudioGraphSettings settings{ winrt::Windows::Media::Render::AudioRenderCategory::Media };
				settings.MaxPlaybackSpeedFactor(1.0);
				winrt::Windows::Media::Audio::CreateAudioGraphResult result = winrt::Windows::Media::Audio::AudioGraph::CreateAsync(settings).get();
				if (result.Status() == winrt::Windows::Media::Audio::AudioGraphCreationStatus::Success)
				{
					m_graph = result.Graph();
					m_graphCreated = true;
	
					winrt::Windows::Media::Audio::CreateAudioDeviceOutputNodeResult oResult = m_graph.CreateDeviceOutputNodeAsync().get();
					if (oResult.Status() == winrt::Windows::Media::Audio::AudioDeviceNodeCreationStatus::Success)
					{
						m_outputNode = oResult.DeviceOutputNode();
						m_outputNodeCreated = true;
					}
				}
				else if (outputNode == nullptr)
				{
					winrt::Windows::Media::Audio::CreateAudioDeviceOutputNodeResult oResult = m_graph.CreateDeviceOutputNodeAsync().get();
					if (oResult.Status() == winrt::Windows::Media::Audio::AudioDeviceNodeCreationStatus::Success)
					{
						m_outputNode = oResult.DeviceOutputNode();
						m_outputNodeCreated = true;
					}
				}
				m_graph.Start();
#ifdef EXCEPTION
	}
	catch (std::exception& ex)
	{
		MessageBoxA(nullptr, ex.what(), std::source_location::current().function_name(), MB_OK);
	}
	catch (winrt::hresult_error& hr)
	{
		std::wstringstream ss1,ss2;
		ss1 << L"エラーコード：" << std::hex << hr.code() << L"\n" << hr.message().c_str();
		ss2 << std::source_location::current().function_name();
		MessageBox(nullptr, ss1.str().c_str(), ss2.str().c_str(), MB_OK);
	}
#else
			catch (...) {}
#endif
		} };
		if(th.joinable())th.join();
	}
#ifdef EXCEPTION
	}
	catch (std::exception& ex)
	{
		MessageBoxA(nullptr, ex.what(), std::source_location::current().function_name(), MB_OK);
	}
	catch (winrt::hresult_error& hr)
	{
		std::wstringstream ss1, ss2;
		ss1 << L"エラーコード：" << std::hex << hr.code() << L"\n" << hr.message().c_str();
		ss2 << std::source_location::current().function_name();
		MessageBox(nullptr, ss1.str().c_str(), ss2.str().c_str(), MB_OK);
	}
#endif // EXCEPTION
}

CAutoAnnounce::~CAutoAnnounce()noexcept
{
	if (m_thread1.joinable())
	{
		m_thread1.join();
	}
	if (m_thread2.joinable())
	{
		m_thread2.join();
	}
	std::thread th{ [&]()
	{
		m_graph.Stop();
		m_Announce1=nullptr;
		m_Announce2=nullptr;
		if (m_outputNodeCreated)
		{
			if (m_outputNode)m_outputNode=nullptr;
		}
		if (m_graphCreated)
		{
			if (m_graph)m_graph=nullptr;
		}
	} };
	if(th.joinable())th.join();
}

void CAutoAnnounce::setTrainNo(int number)
{
#ifdef EXCEPTION
	try {
#endif // EXCEPTION
	m_trainNo = number;
	m_first = AnnounceSet{};
	m_A_Set.clear();
	std::wifstream table(m_table_dir / (std::to_wstring(m_trainNo) + L".csv"));
	table.imbue(std::locale(".UTF-8"));
//	std::wofstream ofs(m_module_dir + L"anndebug.txt");
	if (!table.fail())
	{
		while (!table.eof())
		{
			std::wstring loadline;
			std::getline(table, loadline);//行を読み込む
			std::vector<std::wstring> columun;
			cleanUpBveStr(loadline, table.getloc());//コメントやスペースを消去
			if (!loadline.empty())//何も書いていないかコメントだけの時は何もしない。
			{
				splitCsvColumn(loadline, columun, table.getloc());//行をセルごとに分ける。
				if (columun.size() > 0)
				{
					if (columun.at(0) == L"first")//始発駅
					{
						m_first.sta_no = 0;//なんでもいい
						if (columun.size() > 1)
						{
							if (!columun.at(1).empty())m_first.name1 = m_table_dir / columun.at(1);//発車アナウンス
						}
						if (columun.size() > 2)
						{
							if (columun.at(2) == L"COM")m_first.mode = AnnounceMode::Commuter;
							else if (columun.at(2) == L"R")m_first.mode = AnnounceMode::Rapit;
							else if (columun.at(2) == L"SP")m_first.mode = AnnounceMode::Southern;
							else if (columun.at(2) == L"K")m_first.mode = AnnounceMode::Koya;
							else if (!columun.at(2).empty() && std::all_of(columun.at(2).cbegin(), columun.at(2).cend(), isdigit))
							{
								m_first.mode = AnnounceMode::manual;
								m_first.location1 = std::stod(columun.at(2));
							}
							else m_first.mode = AnnounceMode::Commuter;
						}
						if (columun.size() > 3)
						{
							if (!columun.at(3).empty())m_first.name2 = m_table_dir / columun.at(3);//到着アナウンス
						}
						if (columun.size() > 4)
						{
							if (!columun.at(4).empty() && std::all_of(columun.at(4).cbegin(), columun.at(4).cend(), isdigit))m_first.location2 = std::stod(columun.at(4));//到着アナウンス距離程
							else m_first.location2 = std::numeric_limits<double>::max();
						}
						//						ofs << std::to_wstring(m_first.sta_no) << L"," << m_first.name1 << L"," << m_first.name2 << L"," << std::to_wstring(m_first.location2) << std::endl;
					}
					else
					{
						AnnounceSet buf;
						if (!columun.at(0).empty() && (std::all_of(columun.at(0).cbegin(), columun.at(0).cend(), isdigit)))buf.sta_no = std::stoi(columun.at(0));//駅番号
						else buf.sta_no = std::numeric_limits<int>::max();
						if (columun.size() > 1)
						{
							if (!columun.at(1).empty())buf.name1 = m_table_dir / columun.at(1);//発車アナウンス		if (columun.size() > 2)
						}
						if (columun.size() > 2)
						{
							if (columun.at(2) == L"COM")buf.mode = AnnounceMode::Commuter;
							else if (columun.at(2) == L"R")buf.mode = AnnounceMode::Rapit;
							else if (columun.at(2) == L"SP")buf.mode = AnnounceMode::Southern;
							else if (columun.at(2) == L"K")buf.mode = AnnounceMode::Koya;
							else if (!columun.at(2).empty() && std::all_of(columun.at(2).cbegin(), columun.at(2).cend(), isdigit))
							{
								buf.mode = AnnounceMode::manual;
								buf.location1 = std::stod(columun.at(2));
							}
							else buf.mode = AnnounceMode::Commuter;
						}
						if (columun.size() > 3)
						{
							if (!columun.at(3).empty())buf.name2 = m_table_dir / columun.at(3);//到着アナウンス
						}
						if (columun.size() > 4)
						{
							if (!columun.at(4).empty() && std::all_of(columun.at(4).cbegin(), columun.at(4).cend(), isdigit))buf.location2 = std::stod(columun.at(4));//到着アナウンス距離程
							else buf.location2 = std::numeric_limits<double>::max();
						}
						//						ofs << std::to_wstring(buf.sta_no) << L"," << buf.name1 << L"," << buf.name2 << L"," << std::to_wstring(buf.dist2) << std::endl;
						m_A_Set.emplace_back(std::move(buf));
					}
				}
			}
		}
		table.close();
//		ofs.close();
	}
	m_A_Set.shrink_to_fit();
#ifdef EXCEPTION
	}
	catch (std::exception& ex)
	{
		MessageBoxA(nullptr, ex.what(), std::source_location::current().function_name(), MB_OK);
	}
	catch (winrt::hresult_error& hr)
	{
		std::wstringstream ss1, ss2;
		ss1 << L"エラーコード：" << std::hex << hr.code() << L"\n" << hr.message().c_str();
		ss2 << std::source_location::current().function_name();
		MessageBox(nullptr, ss1.str().c_str(), ss2.str().c_str(), MB_OK);
	}
#endif // EXCEPTION
}



