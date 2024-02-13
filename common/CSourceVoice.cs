using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using SlimDX.XAudio2;
using System.Runtime.InteropServices;


namespace AtsPlugin
{
	internal class CSourceVoice
	{
		//メンバ変数
		private SlimDX.XAudio2.XAudio2 m_pXAudio2;//IXAudio2へのポインタ
		private SlimDX.XAudio2.SourceVoice m_pSourceVoice;//ソースボイスへのポインタ
		private byte[] m_audioData;//音声データを保存する領域
		private int m_LoopCount = XAudio2.LoopInfinite;///ループカウント（1回の場合は0，2回の場合は1，…省略した場合は無限ループ
		private SlimDX.XAudio2.VoiceFlags m_Flags = 0x0;//フラグ
		private float m_MaxFrequencyRatio = SlimDX.XAudio2.XAudio2.DefaultFrequencyRatio;//ピッチの最大値，2
		private IntPtr m_pCallback;//コールバックへのポインタ，省略可
		private SlimDX.XAudio2.VoiceSendDescriptor[] m_pSendList;//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
		private SlimDX.XAudio2.EffectDescriptor[] m_pEffectChain;//エフェクトチェーン構造体へのポインタ，省略可
		private SlimDX.XAudio2.AudioBuffer m_pBuffer;//XAUDIO2_BUFFER構造体
		private SlimDX.Multimedia.WaveFormat m_pWfx;//ソースボイスに渡す形式
		private bool m_started = false;//startしてからstopするまでtrue


		private static int bitPerByte = (int)Math.Log((int)byte.MaxValue + 1, 2);



		//trueの時に開始するなど（外部から自由に使う）
		public bool flag = false; protected const int WM_APP = 0x8000;
		public const int WM_APP_PLAYER_EVENT = WM_APP + 1;

		private const int MF_SDK_VERSION = 0x0001;
		private const int MF_API_VERSION = 0x0070;
		public const int MF_VERSION = (MF_SDK_VERSION << 16 | MF_API_VERSION);

		//再生
		public SlimDX.Result Start(
		int OperationSet = XAudio2.CommitNow//オペレーションセット（省略可）
			)
		{
			if (m_LoopCount == XAudio2.LoopInfinite)
			{
				return StartInf(OperationSet);
			}
			else return StartBound(OperationSet);
		}
		//停止
		public SlimDX.Result Stop(
				int OperationSet = XAudio2.CommitNow//オペレーションセット（省略可）
			)
		{
			if (m_LoopCount == XAudio2.LoopInfinite)
			{
				return StopInf(OperationSet);
			}
			else return StopBound(OperationSet);
		}

		//音量を0から1の間で設定
		public void SetVolume_0_1(
				float vol,//音量（1でオリジナルの音量）
				int OperationSet = XAudio2.CommitNow//オペレーションセット（省略可）
			)
		{
			if (m_pSourceVoice != null) m_pSourceVoice.Volume = Math.Max(Math.Min(vol, 0.0f), 1.0f);
		}
		//その他のIXaudio2SourceVoiceに実装されている関数
		public SlimDX.Result DisableEffect(int EffectIndex, int OperationSet = XAudio2.CommitNow)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.DisableEffect(EffectIndex, OperationSet);
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		public SlimDX.Result Discontinuity()
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.Discontinuity();
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		public SlimDX.Result EnableEffect(int EffectIndex, int OperationSet = XAudio2.CommitNow)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.EnableEffect(EffectIndex, OperationSet);
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		public SlimDX.Result ExitLoop(int OperationSet = XAudio2.CommitNow)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.ExitLoop(OperationSet);
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		public float[] GetChannelVolumes(int Channels)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.GetChannelVolumes(Channels);
			else return new float[Channels];
		}
		public int GetEffectParameters(int EffectIndex)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.GetEffectParameters<int>(EffectIndex);
			else return 0;
		}
		public bool GetEffectState(int EffectIndex)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.IsEffectEnabled(EffectIndex);
			else return false;
		}
		public FilterParameters GetFilterParameters()
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.FilterParameters;
			else return new FilterParameters();
		}
		public float GetFrequencyRatio()
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.FrequencyRatio;
			else return 0.0f;
		}
		public FilterParameters GetOutputFilterParameters(Voice pDestinationVoice)
		{
			if (m_pSourceVoice != null) return pDestinationVoice.FilterParameters;
			else return new FilterParameters();
		}
		public float[] GetOutputMatrix(Voice pDestinationVoice, int SourceChannels, int DestinatioChannels)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.GetOutputMatrix(pDestinationVoice, SourceChannels, DestinatioChannels);
			else return new float[SourceChannels];
		}
		public VoiceState GetState()
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.State;
			else return new VoiceState();
		}
		public VoiceDetails GetVoiceDetails()
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.VoiceDetails;
			else return new VoiceDetails();
		}
		public float GetVolume()
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.Volume;
			else return 0.0f;
		}
		public SlimDX.Result SetChannelVolumes(int Channels, float[] pVolumes, int OperationSet = XAudio2.CommitNow)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.SetChannelVolumes(Channels, pVolumes, OperationSet);
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		public SlimDX.Result SetEffectChain(EffectDescriptor[] pEffectChain)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.SetEffectChain(pEffectChain);
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		public SlimDX.Result SetEffectParameters<T>(int EffectIndex, FilterParameters pParameters, int OperationSet = XAudio2.CommitNow)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.SetEffectParameters(EffectIndex, pParameters, OperationSet);
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		public void SetFilterParameters(FilterParameters Parameters, int OperationSet = XAudio2.CommitNow)
		{
			if (m_pSourceVoice != null) m_pSourceVoice.FilterParameters = Parameters;
		}
		public void SetFrequencyRatio(float ratio, int OperationSet = XAudio2.CommitNow)
		{
			if (m_pSourceVoice != null) m_pSourceVoice.FrequencyRatio = ratio;
		}
		/*	public SlimDX.Result SetOutputFilterParameters(Voice pDestinationVoice, FilterParameters[] pParameters, int OperationSet = XAudio2.CommitNow)
			{
				if (m_pSourceVoice != null)return m_pSourceVoice.
				else return SlimDX.XAudio2.ResultCode.InvalidCall;
			}*/
		public SlimDX.Result SetOutputMatrix(Voice pDestinationVoice, int SourceChannels, int DestinatioChannels, float[] pLevelMatrix, int OperationSet = XAudio2.CommitNow)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.SetOutputMatrix(pDestinationVoice, SourceChannels, DestinatioChannels, pLevelMatrix, OperationSet);
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		public SlimDX.Result SetOutputVoices(SlimDX.XAudio2.VoiceSendDescriptor[] pSendList)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.SetOutputVoices(pSendList);
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		public SlimDX.Result SetSourceSampleRate(int NewSourceSampleRate)
		{
			if (m_pSourceVoice != null) return m_pSourceVoice.SetSourceSampleRate(NewSourceSampleRate);
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		public void SetVolume(float vol, int OperationSet = XAudio2.CommitNow)
		{
			if (m_pSourceVoice != null) m_pSourceVoice.Volume = vol;
		}

		//再生中かどうか
		public bool isRunning()
		{
			if (m_LoopCount == XAudio2.LoopInfinite)
			{
				return m_started;
			}
			else
			{
				return m_pSourceVoice.State.BuffersQueued > 0 && m_started;
			}
		}
		void Destroy_Voice()
		{
			if (m_pSourceVoice != null)
			{
				m_pSourceVoice.Dispose();
				m_pSourceVoice = null;
			}
		}


		//無限ループ時の再生
		private SlimDX.Result StartInf(int OperationSet)
		{
			if (m_pSourceVoice != null)
			{
				SlimDX.Result hr = m_pSourceVoice.Start(PlayFlags.None, OperationSet);
				if (hr.IsSuccess) m_started = true;
				return hr;
			}
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		//再生回数有限時の再生
		private SlimDX.Result StartBound(int OperationSet)
		{
			SlimDX.Result hr = SlimDX.XAudio2.ResultCode.Success;
			if (m_pSourceVoice != null)
			{
				if (isRunning())//再生途中の場合
				{
					hr = StopBound(XAudio2.CommitNow);
					if (hr.IsFailure) return hr;//初めから再生しなおすために，止めてバッファを消去する。(OperationSetを使わず，直ちに停止）
				}
				if (m_pBuffer != null)
				{
					m_pBuffer.AudioData.Position = 0;
					hr = m_pSourceVoice.SubmitSourceBuffer(m_pBuffer);
					if (hr.IsFailure) return hr;//バッファをにデータを入れる。
				}
				if (!m_started)//もし，停止していた場合，再生する。
				{
					hr = m_pSourceVoice.Start(PlayFlags.None, OperationSet);
					if (hr.IsSuccess) m_started = true;
				}
				return hr;
			}
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}

		//無限ループ時の停止
		private SlimDX.Result StopInf(int OperationSet)
		{
			if (m_pSourceVoice != null)
			{
				SlimDX.Result hr = m_pSourceVoice.Stop(PlayFlags.None, OperationSet);
				if (hr.IsSuccess) m_started = false;
				return hr;
			}
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		//再生回数有限時の停止
		private SlimDX.Result StopBound(int OperationSet)
		{
			if (m_pSourceVoice != null)
			{
				if (m_started)
				{
					SlimDX.Result hr = m_pSourceVoice.Stop(PlayFlags.None, OperationSet);
					if (hr.IsSuccess) m_started = false;
					else return hr;
				}
				return m_pSourceVoice.FlushSourceBuffers();
			}
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}

		//コンストラクタ
		public CSourceVoice()
		{
			if (m_started) Stop();
			Destroy_Voice();
			m_pXAudio2 = null;
			m_LoopCount = XAudio2.LoopInfinite;
			m_audioData = null;
			m_pWfx = null;
			m_pBuffer = null;
			m_started = false;
			flag = false;
			m_Flags = VoiceFlags.None;
			m_MaxFrequencyRatio = XAudio2.DefaultFrequencyRatio;
			m_pSendList = null;
			m_pEffectChain = null;
		}
		public CSourceVoice(
			SlimDX.XAudio2.XAudio2 Xau2,//IXAudio2インターフェースへのポインタ 
			StreamWriter fp,//初期化ログファイルへのポインタ
			string name,//音声ファイルのファイル名
			int LoopCount = XAudio2.LoopInfinite,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
			SlimDX.XAudio2.VoiceFlags Flags = 0x0,//フラグ
			float MaxFrequencyRatio = XAudio2.DefaultFrequencyRatio,//ピッチの最大値，省略した場合は2
			SlimDX.XAudio2.VoiceSendDescriptor[] pSendList = null,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
			SlimDX.XAudio2.EffectDescriptor[] pEffectChain = null//エフェクトチェーン構造体へのポインタ，省略可
		)
		{
			if (m_started) Stop();
			Destroy_Voice();
			m_audioData = null;
			m_pWfx = null;
			m_pBuffer = null;
			m_pXAudio2 = Xau2;
			m_LoopCount = LoopCount;
			flag = false;
			m_Flags = Flags;
			m_MaxFrequencyRatio = MaxFrequencyRatio;
			m_pSendList = pSendList;
			m_pEffectChain = pEffectChain;
			CreateSourceVoice(name, fp);
		}
		public CSourceVoice(
			SlimDX.XAudio2.XAudio2 Xau2,//IXAudio2インターフェースへのポインタ 
			string name,//音声ファイルのファイル名
			int LoopCount = XAudio2.LoopInfinite,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
			SlimDX.XAudio2.VoiceFlags Flags = 0x0,//フラグ
			float MaxFrequencyRatio = XAudio2.DefaultFrequencyRatio,//ピッチの最大値，省略した場合は2
			SlimDX.XAudio2.VoiceSendDescriptor[] pSendList = null,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
			SlimDX.XAudio2.EffectDescriptor[] pEffectChain = null//エフェクトチェーン構造体へのポインタ，省略可
		)
		{
			if (m_started) Stop();
			Destroy_Voice();
			m_audioData = null;
			m_pWfx = null;
			m_pBuffer = null;
			m_pXAudio2 = Xau2;
			m_LoopCount = LoopCount;
			flag = false;
			m_Flags = Flags;
			m_MaxFrequencyRatio = MaxFrequencyRatio;
			m_pSendList = pSendList;
			m_pEffectChain = pEffectChain;
			CreateSourceVoice(name, null);
		}
		public void reset()
		{
			if (m_started) Stop();
			Destroy_Voice();
			m_pXAudio2 = null;
			m_LoopCount = XAudio2.LoopInfinite;
			m_audioData = null;
			m_pWfx = null;
			m_pBuffer = null;
			m_started = false;
			flag = false;
			m_Flags = VoiceFlags.None;
			m_MaxFrequencyRatio = XAudio2.DefaultFrequencyRatio;
			m_pSendList = null;
			m_pEffectChain = null;
		}

		//コンストラクタと同じ役割をする。
		public void reset(
			SlimDX.XAudio2.XAudio2 Xau2,//IXAudio2インターフェースへのポインタ 
			string name,//音声ファイルのファイル名
			int LoopCount = XAudio2.LoopInfinite,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
			SlimDX.XAudio2.VoiceFlags Flags = 0x0,//フラグ
			float MaxFrequencyRatio = XAudio2.DefaultFrequencyRatio,//ピッチの最大値，省略した場合は2
			SlimDX.XAudio2.VoiceSendDescriptor[] pSendList = null,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
			SlimDX.XAudio2.EffectDescriptor[] pEffectChain = null//エフェクトチェーン構造体へのポインタ，省略可
		)
		{
			reset(Xau2, null, name, LoopCount, Flags, MaxFrequencyRatio, pSendList, pEffectChain);
		}
		//コンストラクタと同じ役割をする。
		public void reset(
			SlimDX.XAudio2.XAudio2 Xau2,//IXAudio2インターフェースへのポインタ 
			StreamWriter fp,//初期化ログファイルへのポインタ
			string name,//音声ファイルのファイル名
			int LoopCount = XAudio2.LoopInfinite,//ループカウント（1回の場合は0，2回の場合は1，…，省略した場合は無限ループ
			SlimDX.XAudio2.VoiceFlags Flags = 0x0,//フラグ
			float MaxFrequencyRatio = XAudio2.DefaultFrequencyRatio,//ピッチの最大値，省略した場合は2
			SlimDX.XAudio2.VoiceSendDescriptor[] pSendList = null,//XAUDIO2_VOICE_SENDS構造体へのポインタ，省略可
			SlimDX.XAudio2.EffectDescriptor[] pEffectChain = null//エフェクトチェーン構造体へのポインタ，省略可
		)
		{
			if (m_started) Stop();
			Destroy_Voice();
			m_audioData = null;
			m_pWfx = null;
			m_pBuffer = null;
			m_pXAudio2 = Xau2;
			m_LoopCount = LoopCount;
			flag = false;
			m_Flags = Flags;
			m_MaxFrequencyRatio = MaxFrequencyRatio;
			m_pSendList = pSendList;
			m_pEffectChain = pEffectChain;
			CreateSourceVoice(name, fp);
		}

		public SlimDX.Result Cue(
		int OperationSet = XAudio2.CommitNow//オペレーションセット（省略可）
	)
		{
			SlimDX.Result hr = SlimDX.XAudio2.ResultCode.Success;
			if (m_pSourceVoice != null)
			{
				if (m_started)//再生途中の場合
				{
					hr = m_pSourceVoice.Stop(PlayFlags.None, XAudio2.CommitNow);
					if (hr.IsFailure) return hr;//初めから再生しなおすために，止める。(OperationSetを使わず，直ちに停止）
				}
				hr = m_pSourceVoice.FlushSourceBuffers();
				if (hr.IsFailure) return hr;//バッファを消去する。
				if (m_pBuffer != null)
				{
					m_pBuffer.AudioData.Position = 0;
					hr = m_pSourceVoice.SubmitSourceBuffer(m_pBuffer);
					if (hr.IsFailure) return hr;//バッファをにデータを入れる。
				}
				if (m_started)//もし，直前に再生していた場合，再生する。
				{
					hr = m_pSourceVoice.Start(PlayFlags.None, OperationSet);
					if (hr.IsSuccess) m_started = true;
				}
				return hr;
			}
			else return SlimDX.XAudio2.ResultCode.InvalidCall;
		}
		//現在の音声の振幅を返す。
		public float getLevel()
		{
			if (m_pSourceVoice != null)
			{
				VoiceState state = new VoiceState();
				state = GetState();
				if (state.BuffersQueued > 0)
				{
					int BufferSamples = 0;
					if (m_pWfx.BitsPerSample != 0) BufferSamples = m_audioData.Length / (m_pWfx.BitsPerSample / bitPerByte);
					int sample = (int)state.SamplesPlayed;
					if (BufferSamples != 0) sample %= BufferSamples;
					int sampleNum200Hz = m_pWfx.SamplesPerSecond / 200;
					float val = getSampleAvg(sample);
					float temp;
					for (int i = 0; i < sampleNum200Hz; ++i)
					{
						if (sample >= i * m_pWfx.Channels)
						{
							temp = getSampleAvg(sample - i * m_pWfx.Channels);
							if (temp > val)
							{
								val = temp;
							}
						}

					}
					return val;
				}
				else return 0.0f;
			}
			else return 0.0f;
		}
		//ソースボイスのポインタが有効かどうかを返す
		public static bool operator ==(CSourceVoice right, CSourceVoice left)
		{
			return right.m_pSourceVoice == left.m_pSourceVoice;
		}
		//ソースボイスのポインタが有効かどうかを返す
		public static bool operator !=(CSourceVoice right, CSourceVoice left)
		{
			return right.m_pSourceVoice != left.m_pSourceVoice;
		}
		public override int GetHashCode()
		{
			return 0;
		}
		public override bool Equals(object o)
		{
			return true;
		}

		//バイト数の取得
		int getBytes()
		{
			return m_audioData.Length;
		}
		//SlimDX.Multimedia.WaveFormat構造体の取得
		public SlimDX.Multimedia.WaveFormat getFormat()
		{
			return m_pWfx;
		}
		//XAUDIO2_BUFFER 構造体の設定
		public void setPlayLength(int PlayBegin = 0, int PlayLength = 0, int LoopBegin = 0, int LoopLength = 0, int LoopCount = XAudio2.LoopInfinite)
		{
			if (m_pBuffer != null)
			{
				if (m_pSourceVoice != null)
				{
					m_pSourceVoice.FlushSourceBuffers();
				}
				m_pBuffer.PlayBegin = PlayBegin;
				m_pBuffer.PlayLength = PlayLength;
				m_pBuffer.LoopBegin = LoopBegin;
				m_pBuffer.LoopLength = LoopLength;
				m_pBuffer.LoopCount = LoopCount;
			}
		}
		//XAUDIO2_BUFFER 構造体の再読み込み
		public void reSubmitSourceBuffer()
		{
			if (m_pSourceVoice != null)
			{
				m_pSourceVoice.FlushSourceBuffers();
				m_pBuffer.AudioData.Position = 0;
				m_pSourceVoice.SubmitSourceBuffer(m_pBuffer);
			}
		}
		//一時停止（m_startedとm_bufferを変更せずに停止する。）
		SlimDX.Result pause(int OperationSet = XAudio2.CommitNow)
		{
			return m_pSourceVoice.Stop(PlayFlags.None, OperationSet);
		}
		//一時停止の解除（m_startedとm_bufferを変更せず再生する。）
		SlimDX.Result pauseDefeat(int OperationSet = XAudio2.CommitNow)
		{
			return m_pSourceVoice.Start(PlayFlags.None, OperationSet);
		}

		//pointから始まるサンプルの各チャンネルの平均を返す。
		private float getSampleAvg(int index)
		{
			float ret;
			if (m_pWfx != null)
			{
				int uiBytesPerSample = m_pWfx.BitsPerSample / bitPerByte;//1サンプルのサイズ（バイト単位）
				int uiSamplesPlayedPerChannel = Math.Max(index - 1, 0) / m_pWfx.Channels;//サンプルの数をチャンネル数で割ったもの-1
				int point = uiSamplesPlayedPerChannel * m_pWfx.Channels * uiBytesPerSample;//サンプルの一番初めの要素の位置
				point %= m_audioData.Length;//サンプルの一番初めの要素の位置（2周目以降の可能性があるため，余りを求める）
				switch (m_pWfx.BitsPerSample)
				{
					case 8:
						{
							var Samples = new List<int>(m_pWfx.Channels);
							for (int i = 0; i < m_pWfx.Channels; ++i)
							{
								Samples[i] = m_audioData[point + i];
							}
							int avg = Samples.Sum() / (int)(m_pWfx.Channels);//各チャンネルの平均値
							ret = Math.Abs((float)avg / (float)byte.MaxValue);
						}
						break;
					case 16:
						{
							var Samples = new List<int>(m_pWfx.Channels);
							for (int i = 0; i < m_pWfx.Channels; ++i)
							{
								int temp = 0;
								for (int j = 0; j < uiBytesPerSample; ++j)
								{
									temp <<= bitPerByte;
									temp += m_audioData[point + uiBytesPerSample * i + uiBytesPerSample - 1 - j];//リトルエンディアンのため，後から読み込む
								}
								Samples[i] |= temp;
							}
							int avg = Samples.Sum() / m_pWfx.Channels;//各チャンネルの平均値
							ret = Math.Abs((float)(avg) / (float)Int16.MaxValue);
						}
						break;
					case 24:
						{
							var Samples = new List<Int32>(m_pWfx.Channels);
							for (int i = 0; i < m_pWfx.Channels; ++i)
							{
								Int32 temp = 0;
								for (int j = 0; j < uiBytesPerSample; ++j)
								{
									temp <<= bitPerByte;
									temp += m_audioData[point + uiBytesPerSample * i + uiBytesPerSample - 1 - j];//リトルエンディアンのため，後から読み込む
								}
								temp <<= bitPerByte;//32ビットにするため，左に詰め，下位8ビットは0になる。
								Samples[i] |= temp;
							}
							int avg = Samples.Sum() / m_pWfx.Channels;//各チャンネルの平均値
							ret = Math.Abs((float)avg / (float)Int32.MaxValue);//32ビット変数のため，32ビットの最大値で割る。
						}
						break;
					case 32:
						{
							var Samples = new List<Int32>(m_pWfx.Channels);
							for (int i = 0; i < m_pWfx.Channels; ++i)
							{
								Int32 temp = 0;
								for (int j = 0; j < uiBytesPerSample; ++j)
								{
									temp <<= bitPerByte;
									temp += m_audioData[point + uiBytesPerSample * i + uiBytesPerSample - 1 - j];//リトルエンディアンのため，後から読み込む
								}
								temp <<= bitPerByte;//32ビットにするため，左に詰め，下位8ビットは0になる。
								Samples[i] |= temp;
							}
							int avg = Samples.Sum() / m_pWfx.Channels;//各チャンネルの平均値
							ret = Math.Abs((float)avg / (float)Int32.MaxValue);//32ビット変数のため，32ビットの最大値で割る。
						}
						break;
					default:
						ret = 1.0f;
						break;
				}
			}
			else ret = 0.0f;
			return ret;
		}
		//音声をバッファに読み込み，ソースボイスを作る。
		void CreateSourceVoice(
			string name,//音声データのファイル名
			StreamWriter fp = null//初期化ログファイルへのポインタ
		)
		{
			string name_normal = new System.IO.FileInfo(name).FullName;

			try
			{
				var stream = File.OpenRead(name_normal);
				SlimDX.Multimedia.WaveStream wave = new SlimDX.Multimedia.WaveStream(stream);
				stream.Close();
				stream.Dispose();

				Array.Resize(ref m_audioData, (int)wave.Length);
				wave.Read(m_audioData, 0, (int)wave.Length);
				m_pWfx = wave.Format;
				wave.Close();
				wave.Dispose();

				m_pSourceVoice = new SourceVoice(m_pXAudio2, m_pWfx);

				m_pBuffer = new AudioBuffer
				{
					AudioData = new System.IO.MemoryStream(m_audioData),
					AudioBytes = m_audioData.Length,
					Flags = SlimDX.XAudio2.BufferFlags.EndOfStream,
					LoopCount = m_LoopCount,
				};

				if (m_LoopCount == XAudio2.LoopInfinite)//ループ再生の時のみ
				{
					//バッファをソースボイスに登録する。
					m_pSourceVoice.SubmitSourceBuffer(m_pBuffer);
					Start();
					SetVolume(0.0f);
				}
			}
			catch (SlimDX.SlimDXException ex)
			{
				reset();
				if (fp != null) fp.WriteLine("ソースボイスの作成失敗\nエラー:" + ex.ToString());
				if (fp != null) fp.WriteLine("ファイル名：" + name_normal);
				return;
			}
            catch (System.Exception ex)
            {
                reset();
                if (fp != null) fp.WriteLine("ソースボイスの作成失敗\nエラー:" + ex.ToString());
                if (fp != null) fp.WriteLine("ファイル名：" + name_normal);
                return;
            }
            if (fp != null) fp.WriteLine("ソースボイスの作成成功");
			if (fp != null) fp.WriteLine("ファイル名：" + name_normal);
		}

	}
}
