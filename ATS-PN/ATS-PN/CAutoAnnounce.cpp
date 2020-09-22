#pragma once
#include "pch.h"
#include "CAutoAnnounce.h"

CAutoAnnounce::CAutoAnnounce(const std::wstring& moduleDir, IXAudio2* pXau2) :
	m_trainNo(0),//時刻表番号
	m_staNo(0),//駅番号
	m_Location(0.0), m_Location_pre(0.0),//距離程
	//	m_LocationOrigin = 0.0;//扉が閉まった瞬間の距離程
	m_AnnounceMode(AnnounceMode::Commuter),
	//	m_RunDistance = 0.0, m_RunDistance_pre = 0.0;//出発してからの距離
	m_set_no(false),//一度でも105番地上子を踏んだことがあったらtrue
	m_A_Loc1(DBL_MAX),//出発放送の距離程を保存
	m_A_Loc2(DBL_MAX),//到着放送の距離程を保存
	m_pXAudio2(pXau2),
	m_module_dir(moduleDir),
	m_table_dir(moduleDir + L"announce\\") 
{}

CAutoAnnounce::~CAutoAnnounce()
{
	SAFE_DELETE(m_Announce1);
	SAFE_DELETE(m_Announce2);
}

void CAutoAnnounce::setTrainNo(int number)
{
	m_trainNo = number;
	std::wifstream table(m_table_dir + std::to_wstring(m_trainNo) + L".csv");
	table.imbue(std::locale("ja-JP"));
//	std::wofstream ofs(m_module_dir + L"anndebug.txt");
	if (!table.fail())
	{
		while (!table.eof())
		{
			std::wstring loadline;
			std::getline(table, loadline);//行を読み込む
			std::vector<std::wstring> columun;
			cleanUpBveStr(&loadline);//コメントやスペースを消去
			if (!loadline.empty())//何も書いていないかコメントだけの時は何もしない。
			{
				splitCsvColumn(loadline, &columun);//行をセルごとに分ける。

				if (columun.at(0) == L"first")//始発駅
				{
					m_first.sta_no = 0;//なんでもいい
					if (columun.size() > 1)if (!columun.at(1).empty())m_first.name1 = m_table_dir + columun.at(1);//発車アナウンス
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
					if (columun.size() > 3)if (!columun.at(3).empty())m_first.name2 = m_table_dir + columun.at(3);//到着アナウンス
					if (columun.size() > 4)
					{
						if (!columun.at(4).empty() && std::all_of(columun.at(4).cbegin(), columun.at(4).cend(), isdigit))m_first.location2 = std::stod(columun.at(4));//到着アナウンス距離程
						else m_first.location2 = DBL_MAX;
					}
					m_first.name1.shrink_to_fit();
					m_first.name2.shrink_to_fit();
//						ofs << std::to_wstring(m_first.sta_no) << L"," << m_first.name1 << L"," << m_first.name2 << L"," << std::to_wstring(m_first.dist2) << std::endl;
				}
				else
				{
					AnnounceSet buf;
					if (!columun.at(0).empty() && (std::all_of(columun.at(0).cbegin(), columun.at(0).cend(), isdigit)))buf.sta_no = std::stoi(columun.at(0));//駅番号
					else buf.sta_no = INT_MAX;
					if (columun.size() > 1)if (!columun.at(1).empty())buf.name1 = m_table_dir + columun.at(1);//発車アナウンス		if (columun.size() > 2)
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
					if (columun.size() > 3)if (!columun.at(3).empty())buf.name2 = m_table_dir + columun.at(3);//到着アナウンス
					if (columun.size() > 4)
					{
						if (!columun.at(4).empty() && std::all_of(columun.at(4).cbegin(), columun.at(4).cend(), isdigit))buf.location2 = std::stod(columun.at(4));//到着アナウンス距離程
						else buf.location2 = DBL_MAX;
					}
					buf.name1.shrink_to_fit();
					buf.name2.shrink_to_fit();
//						ofs << std::to_wstring(buf.sta_no) << L"," << buf.name1 << L"," << buf.name2 << L"," << std::to_wstring(buf.dist2) << std::endl;
					m_A_Set.emplace_back(buf);
				}
			}
		}
		table.close();
//		ofs.close();
	}
	m_A_Set.shrink_to_fit();
}

void CAutoAnnounce::Running(const double loc, const int time)
{
	static int time_pre = 0;//前フレームの時刻
	int delT = 0;//1フレームの時間
	delT = time - time_pre;
	m_Location = loc;//現在の位置
//	m_RunDistance = m_Location - m_LocationOrigin;//出発してからの距離

	if (delT >= 1000 || delT < 0)//駅に移動したとき
	{
		if (m_Announce1)m_Announce1->Stop();
		if (m_Announce2)m_Announce2->Stop();
		if (m_Location >= m_A_Loc1)SAFE_DELETE(m_Announce1);
		if (m_Location >= m_A_Loc2)SAFE_DELETE(m_Announce2);
	}
	if (m_Announce1)if (m_Location_pre < m_A_Loc1 && m_Location >= m_A_Loc1)m_Announce1->Start();
	if (m_Announce2)if (m_Location_pre < m_A_Loc2 && m_Location >= m_A_Loc2)m_Announce2->Start();


	m_Location_pre = m_Location;
//	m_RunDistance_pre = m_RunDistance;
	time_pre = time;
}

void CAutoAnnounce::Halt(const int no)
{
	if(!m_set_no)m_set_no = true;
	m_staNo = no;
}

void CAutoAnnounce::DoorCls(void)
{
//	m_LocationOrigin = m_Location;//駅発車時の位置を登録（出発後の放送に使用）
	SAFE_DELETE(m_Announce1);//前の放送を消去
	SAFE_DELETE(m_Announce2);//前の放送を消去
	if (m_set_no)//始発駅以外
	{
		for (const auto& a : m_A_Set)
		{
			if (a.sta_no == m_staNo)
			{
				HRESULT hr;
				BOOL mfStarted = FALSE;//メディアファンデーションプラットフォームを初期化したらTRUEにする。
				hr = MFStartup(MF_VERSION);// メディアファンデーションプラットフォームの初期化
				mfStarted = SUCCEEDED(hr);//初期化出来たらTRUEにする。
				if (!a.name1.empty())m_Announce1 = new CSourceVoice(m_pXAudio2, a.name1, 0);//放送を登録
				if (!a.name2.empty())m_Announce2 = new CSourceVoice(m_pXAudio2, a.name2, 0);//放送を登録
				if (mfStarted)MFShutdown();// メディアファンデーションプラットフォームが初期化されていたら終了
				switch (a.mode)//出発後放送の位置を登録
				{
				case AnnounceMode::Commuter:
					m_A_Loc1 = m_Location + DEP_DISTANCE_COM;
					break;
				case AnnounceMode::Rapit:
				case AnnounceMode::Southern:
					m_A_Loc1 = m_Location + DEP_DISTANCE_LIM;
					break;
				case AnnounceMode::Koya:
					m_A_Loc1 = m_Location + DEP_DISTANCE_K;
					break;
				case AnnounceMode::manual:
					m_A_Loc1 = a.location1;
					break;
				}
				m_A_Loc2 = a.location2;//到着前放送の位置を登録
				if (m_Announce1)m_Announce1->SetVolume(1.0f);//音量を1に設定する（これがないと，音量0になる）
				if (m_Announce2)m_Announce2->SetVolume(1.0f);//音量を1に設定する（これがないと，音量0になる）
				break;
			}
		}
	}
	else//始発駅
	{
		HRESULT hr;
		BOOL mfStarted = FALSE;//メディアファンデーションプラットフォームを初期化したらTRUEにする。
		hr = MFStartup(MF_VERSION);// メディアファンデーションプラットフォームの初期化
		mfStarted = SUCCEEDED(hr);//初期化出来たらTRUEにする。
		if (!m_first.name1.empty())m_Announce1 = new CSourceVoice(m_pXAudio2, m_first.name1, 0);//放送を登録
		if (!m_first.name2.empty())m_Announce2 = new CSourceVoice(m_pXAudio2, m_first.name2, 0);//放送を登録
		if (mfStarted)MFShutdown();// メディアファンデーションプラットフォームが初期化されていたら終了
		switch (m_first.mode)//出発後放送の位置を登録
		{
		case AnnounceMode::Commuter:
			m_A_Loc1 = m_Location + DEP_DISTANCE_COM;
			break;
		case AnnounceMode::Rapit:
			m_A_Loc1 = m_Location + DEP_DISTANCE_R_FIRST;
			break;
		case AnnounceMode::Southern:
			m_A_Loc1 = m_Location + DEP_DISTANCE_S_FIRST;
			break;
		case AnnounceMode::Koya:
			m_A_Loc1 = m_Location + DEP_DISTANCE_K;
			break;
		case AnnounceMode::manual:
			m_A_Loc1 = m_first.location1;
			break;
		}
		m_A_Loc2 = m_first.location2;//到着前放送の位置を登録
		if (m_Announce1)m_Announce1->SetVolume(1.0f);//音量を1に設定する（これがないと，音量0になる）
		if (m_Announce2)m_Announce2->SetVolume(1.0f);//音量を1に設定する（これがないと，音量0になる）
	}
}

