using System;

namespace AtsPlugin
{

	internal class CAudioFileInputNode : IEquatable<CAudioFileInputNode>, IDisposable
	{
		//空のコンストラクタ
		public CAudioFileInputNode()
		{
			m_node = null;
			m_started = false;
			flag = false;
		}

		//コピーコンストラクタ
		public CAudioFileInputNode(CAudioFileInputNode right)
		{
			m_node = right.m_node;
			m_started = false;
			flag = false;
		}
		//コンストラクタ
		public CAudioFileInputNode(
			Windows.Media.Audio.AudioGraph graph, //AudioGraphオブジェクトへの参照
			Windows.Media.Audio.AudioDeviceOutputNode outputNode,//OutputNodeへの参照
			string name,//音声ファイルのファイル名
			System.Nullable<int> LoopCount = null//ループカウント（1回の場合は0，2回の場合は1，…，nullは無限ループ
		)
		{
			makeCAudioFileInputNode(graph, outputNode, null, null, name, LoopCount);
		}
		//コンストラクタ
		public CAudioFileInputNode(
   			Windows.Media.Audio.AudioGraph graph, //AudioGraphオブジェクトへの参照
			Windows.Media.Audio.AudioDeviceOutputNode outputNode,//OutputNodeへの参照
			System.Exception pHr,//エラー番号
			string name,//音声ファイルのファイル名
			System.Nullable<int> LoopCount = null//ループカウント（1回の場合は0，2回の場合は1，…，nullは無限ループ
		)
		{
			makeCAudioFileInputNode(graph, outputNode, pHr, null, name, LoopCount);
		}
		//コンストラクタ
		public CAudioFileInputNode(
			Windows.Media.Audio.AudioGraph graph, //AudioGraphオブジェクトへの参照
			Windows.Media.Audio.AudioDeviceOutputNode outputNode,//OutputNodeへの参照
            string logName,//初期化ログファイル名
            string name,//音声ファイルのファイル名
			System.Nullable<int> LoopCount = null//ループカウント（1回の場合は0，2回の場合は1，…，nullは無限ループ
		)
		{
			makeCAudioFileInputNode(graph, outputNode, null, logName, name, LoopCount);
		}
		//コンストラクタ
		public CAudioFileInputNode(
			Windows.Media.Audio.AudioGraph graph, //AudioGraphオブジェクトへの参照
			Windows.Media.Audio.AudioDeviceOutputNode outputNode,//OutputNodeへの参照
			System.Exception pHr,//エラー番号
            string logName,//初期化ログファイル名
            string name,//音声ファイルのファイル名
			System.Nullable<int> LoopCount = null//ループカウント（1回の場合は0，2回の場合は1，…，nullは無限ループ
		)
		{
			makeCAudioFileInputNode(graph, outputNode, pHr, logName, name, LoopCount);
		}

		//デストラクタ
		~CAudioFileInputNode()
		{
            if (m_node != null)
            {
                m_node.Dispose();
            }
            m_started = false;
            flag = false;
        }

        //コピー代入
        //CAudioFileInputNode operator=(const CAudioFileInputNode right) = default;

        //nullの代入
        //CAudioFileInputNode operator=()
        //{
        //	if(m_node)m_node.Stop();
        //	m_node = null;
        //	m_started = false;
        //	flag = false;
        //	return *this;
        //}

        //ファイルインプットノードが有効かどうかを返す
        bool IEquatable<CAudioFileInputNode>.Equals(CAudioFileInputNode other)
        {
			if (other == null)
			{
				return m_node == null;
			}
			else
			{
				throw new NotImplementedException();
			}
        }


        //再生
        public void Start()
		{
			if (m_node != null)
			{
				if (m_node.LoopCount == null)
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
		public void Stop()
		{
			if (m_node != null)
			{
				if (m_node.LoopCount == null)
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
		public void OutgoingGain_0_1(
			double vol//音量（1でオリジナルの音量）
		)
		{
			if (m_node != null)
			{
				if (m_node.LoopCount == null)
				{
					if (vol == 0.0)
					{
						m_node.Stop();
						m_freq = m_node.PlaybackSpeedFactor;
						m_node.PlaybackSpeedFactor = 1.0f;
					}
					else if (m_vol == 0.0 && m_started)
					{
						m_node.Start();
						m_node.PlaybackSpeedFactor = m_freq;
					}
				}
				m_node.OutgoingGain = System.Math.Max(System.Math.Min(vol, 0.0f), 1.0f);
			}
            m_vol = vol;
        }
		//音量を設定
		public void OutgoingGain(
			double vol//音量（1でオリジナルの音量）
		)
		{
			if (m_node != null)
			{
				if (m_node.LoopCount == null)
				{
					if (vol == 0.0)
					{
						m_node.Stop();
						m_freq = m_node.PlaybackSpeedFactor;
						m_node.PlaybackSpeedFactor = 1.0f;
					}
					else if (m_vol == 0.0 && m_started)
					{
						m_node.Start();
						m_node.PlaybackSpeedFactor = m_freq;
					}
                }
                m_node.OutgoingGain = vol;
            }
            m_vol = vol;
        }
		//再生速度を設定
		public void PlaybackSpeedFactor(
			double speed//再生速度（1でオリジナルの速度）
		)
		{
			if (m_node != null) m_node.PlaybackSpeedFactor = speed;
		}
		//開始位置を設定
		public void StartTime(
			System.Nullable<System.TimeSpan> time
		)
		{
			if (m_node != null)
			{
				if (time == null) m_node.StartTime = time;
				else if (time < m_node.Duration) m_node.StartTime = time;
				else m_node.StartTime = m_node.Duration;
			}
		}
		//開始位置を取得
		public System.Nullable<System.TimeSpan> StartTime()
		{
			if (m_node != null) return m_node.StartTime;
			else return null;
		}
		//終了位置を設定
		public void EndTime(
			 System.TimeSpan time
		)
		{
			if (m_node != null)
			{
				if (time == null) m_node.EndTime = time;
				if (time < m_node.Duration) m_node.EndTime = time;
				else m_node.EndTime = null;
			}
		}
		//終了位置を取得
		public System.Nullable<System.TimeSpan> EndTime()
		{
			if (m_node != null) return m_node.EndTime;
			else return null;
		}
		//長さを取得
		public System.TimeSpan Duration()
		{
			if (m_node != null) return m_node.Duration;
			else return new System.TimeSpan(0);
		}
		//繰り返し回数を設定
		public void LoopCount(
			System.Nullable<int> count
		)
		{
			if (m_node != null) m_node.LoopCount = count;
		}
		//繰り返し回数を取得
		public System.Nullable<int> LoopCount()
		{
			if (m_node != null) return m_node.LoopCount;
			else return null;
		}
		public System.TimeSpan Position()
		{
			if (m_node != null) return m_node.Position;
			else return new System.TimeSpan(0);
		}

		//再生中かどうか
		public bool isRunning()

		{
			if (m_node != null)
			{
				if (m_node.LoopCount != null && m_started)
				{
					System.Nullable<System.TimeSpan> endTime = m_node.EndTime;
					System.TimeSpan tsEndTime;
					if (endTime != null) tsEndTime = endTime.Value;
					else tsEndTime = m_node.Duration;
					var msEndTime = tsEndTime.Milliseconds;
					if (m_node.Position.Milliseconds >= msEndTime)
					{
						m_started = false;
					}
				}
				return m_started;
			}
			else return false;
		}
		//頭出し
		public void Reset()
		{
			if (m_node != null) m_node.Reset();
		}
		//頭出し
		public void Cue()
		{
			if (m_node != null)
			{
				m_node.Reset();
				if (m_started) m_node.Start();
			}
		}
		//頭出し
		public void Seek(System.TimeSpan ts)
		{
			if (m_node != null)
			{
				m_node.Seek(ts);
			}
		}
		//一時停止（m_startedとm_bufferを変更せずに停止する。）
		public void pause()
		{
			if (m_node != null) m_node.Stop();
		}
		//一時停止の解除（m_startedとm_bufferを変更せず再生する。）
		public void pauseDefeat()
		{
			if (m_node != null) m_node.Start();
		}
		//終了（消去）
//		public void Close()//例外発生のもとになるので使用しない！！
//		{
//			if (m_node != null)
//			{
//				m_node.Dispose();
//			}
//			m_started = false;
//			flag = false;
//		}
		//trueの時に開始するなど（外部から自由に使う）
		public bool flag;

        public void Dispose()//例外発生のもとになるので使用しない！！
        {
        	if (m_node != null)
        	{
				m_node.Stop();
 //       		m_node.Dispose();
        	}
        	m_started = false;
        	flag = false;
        }

        //コンストラクタ
        private void makeCAudioFileInputNode(
			Windows.Media.Audio.AudioGraph graph, //AudioGraphオブジェクトへの参照
			Windows.Media.Audio.AudioDeviceOutputNode outputNode,//OutputNodeへの参照
			System.Exception pHr,//エラー番号
			string logName,//初期化ログファイル名
            string name,//音声ファイルのファイル名
			System.Nullable<int> LoopCount = null//ループカウント（1回の場合は0，2回の場合は1，…，nullは無限ループ
		)
		{
			m_node = null;
			m_started = false;
			flag = false;

			System.Exception hr = CreateAudioFileInputNode(name, graph, outputNode, LoopCount, logName);
			if (pHr != null) pHr = hr;
		}

		Windows.Media.Audio.AudioFileInputNode m_node;
		bool m_started;
		double m_vol = 0.0f;
		double m_freq = 1.0f;


		private System.Exception CreateAudioFileInputNode(
			string name,//音声データのファイル名
			Windows.Media.Audio.AudioGraph graph,
			Windows.Media.Audio.AudioDeviceOutputNode outputNode,
			System.Nullable<int> LoopCount = null,
			string logName = null//初期化ログファイル名
		)
		{
			Windows.Media.Audio.CreateAudioFileInputNodeResult result = null;
			var th = new System.Threading.Thread(async () =>
            {
                string name_normal = new System.IO.FileInfo(name).FullName;
                try
                {
                    var storageFile = await Windows.Storage.StorageFile.GetFileFromPathAsync(name_normal);
                    var fResult = await graph.CreateFileInputNodeAsync(storageFile);
					if (fResult.Status == Windows.Media.Audio.AudioFileNodeCreationStatus.Success)//作成成功
					{
						fResult.FileInputNode.Stop();
						fResult.FileInputNode.AddOutgoingConnection(outputNode);
						fResult.FileInputNode.LoopCount = LoopCount;
						m_node = fResult.FileInputNode;
						if (LoopCount == null)//ループ再生時
						{
							Start();
							OutgoingGain(0.0);
						}
					}
					result = fResult;
				}
				catch (System.Exception hr)
				{
					if (logName != null)
                    {
						System.IO.StreamWriter fp1 = new System.IO.StreamWriter(logName, true);
						await fp1.WriteLineAsync("AudioFileInputNode作成失敗");
						await fp1.WriteLineAsync(hr.ToString());
						fp1.Close();
					}
				}
                if (logName != null)
                {
                    System.IO.StreamWriter fp1 = new System.IO.StreamWriter(logName, true);
                    await fp1.WriteLineAsync("AudioFileInputNodeの作成成功");
                    await fp1.WriteLineAsync("ファイル名：" + name_normal);
                    fp1.Close();
                }
            });
			th.Start();
			if(th.ThreadState != System.Threading.ThreadState.Unstarted) th.Join();

            if (result != null) return result.ExtendedError;
			else return new System.Exception("AudioFileInputNode作成失敗");
		}

    }
}