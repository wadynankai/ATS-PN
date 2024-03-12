using System;

namespace AtsPlugin
{

	internal class CAudioFileInputNode : IEquatable<CAudioFileInputNode>, IDisposable
	{
		//��̃R���X�g���N�^
		public CAudioFileInputNode()
		{
			m_node = null;
			m_started = false;
			flag = false;
		}

		//�R�s�[�R���X�g���N�^
		public CAudioFileInputNode(CAudioFileInputNode right)
		{
			m_node = right.m_node;
			m_started = false;
			flag = false;
		}
		//�R���X�g���N�^
		public CAudioFileInputNode(
			Windows.Media.Audio.AudioGraph graph, //AudioGraph�I�u�W�F�N�g�ւ̎Q��
			Windows.Media.Audio.AudioDeviceOutputNode outputNode,//OutputNode�ւ̎Q��
			string name,//�����t�@�C���̃t�@�C����
			System.Nullable<int> LoopCount = null//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�Cnull�͖������[�v
		)
		{
			makeCAudioFileInputNode(graph, outputNode, null, null, name, LoopCount);
		}
		//�R���X�g���N�^
		public CAudioFileInputNode(
   			Windows.Media.Audio.AudioGraph graph, //AudioGraph�I�u�W�F�N�g�ւ̎Q��
			Windows.Media.Audio.AudioDeviceOutputNode outputNode,//OutputNode�ւ̎Q��
			System.Exception pHr,//�G���[�ԍ�
			string name,//�����t�@�C���̃t�@�C����
			System.Nullable<int> LoopCount = null//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�Cnull�͖������[�v
		)
		{
			makeCAudioFileInputNode(graph, outputNode, pHr, null, name, LoopCount);
		}
		//�R���X�g���N�^
		public CAudioFileInputNode(
			Windows.Media.Audio.AudioGraph graph, //AudioGraph�I�u�W�F�N�g�ւ̎Q��
			Windows.Media.Audio.AudioDeviceOutputNode outputNode,//OutputNode�ւ̎Q��
            string logName,//���������O�t�@�C����
            string name,//�����t�@�C���̃t�@�C����
			System.Nullable<int> LoopCount = null//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�Cnull�͖������[�v
		)
		{
			makeCAudioFileInputNode(graph, outputNode, null, logName, name, LoopCount);
		}
		//�R���X�g���N�^
		public CAudioFileInputNode(
			Windows.Media.Audio.AudioGraph graph, //AudioGraph�I�u�W�F�N�g�ւ̎Q��
			Windows.Media.Audio.AudioDeviceOutputNode outputNode,//OutputNode�ւ̎Q��
			System.Exception pHr,//�G���[�ԍ�
            string logName,//���������O�t�@�C����
            string name,//�����t�@�C���̃t�@�C����
			System.Nullable<int> LoopCount = null//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�Cnull�͖������[�v
		)
		{
			makeCAudioFileInputNode(graph, outputNode, pHr, logName, name, LoopCount);
		}

		//�f�X�g���N�^
		~CAudioFileInputNode()
		{
            if (m_node != null)
            {
                m_node.Dispose();
            }
            m_started = false;
            flag = false;
        }

        //�R�s�[���
        //CAudioFileInputNode operator=(const CAudioFileInputNode right) = default;

        //null�̑��
        //CAudioFileInputNode operator=()
        //{
        //	if(m_node)m_node.Stop();
        //	m_node = null;
        //	m_started = false;
        //	flag = false;
        //	return *this;
        //}

        //�t�@�C���C���v�b�g�m�[�h���L�����ǂ�����Ԃ�
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


        //�Đ�
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

		//���ʂ�0����1�̊ԂŐݒ�
		public void OutgoingGain_0_1(
			double vol//���ʁi1�ŃI���W�i���̉��ʁj
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
		//���ʂ�ݒ�
		public void OutgoingGain(
			double vol//���ʁi1�ŃI���W�i���̉��ʁj
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
		//�Đ����x��ݒ�
		public void PlaybackSpeedFactor(
			double speed//�Đ����x�i1�ŃI���W�i���̑��x�j
		)
		{
			if (m_node != null) m_node.PlaybackSpeedFactor = speed;
		}
		//�J�n�ʒu��ݒ�
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
		//�J�n�ʒu���擾
		public System.Nullable<System.TimeSpan> StartTime()
		{
			if (m_node != null) return m_node.StartTime;
			else return null;
		}
		//�I���ʒu��ݒ�
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
		//�I���ʒu���擾
		public System.Nullable<System.TimeSpan> EndTime()
		{
			if (m_node != null) return m_node.EndTime;
			else return null;
		}
		//�������擾
		public System.TimeSpan Duration()
		{
			if (m_node != null) return m_node.Duration;
			else return new System.TimeSpan(0);
		}
		//�J��Ԃ��񐔂�ݒ�
		public void LoopCount(
			System.Nullable<int> count
		)
		{
			if (m_node != null) m_node.LoopCount = count;
		}
		//�J��Ԃ��񐔂��擾
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

		//�Đ������ǂ���
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
		//���o��
		public void Reset()
		{
			if (m_node != null) m_node.Reset();
		}
		//���o��
		public void Cue()
		{
			if (m_node != null)
			{
				m_node.Reset();
				if (m_started) m_node.Start();
			}
		}
		//���o��
		public void Seek(System.TimeSpan ts)
		{
			if (m_node != null)
			{
				m_node.Seek(ts);
			}
		}
		//�ꎞ��~�im_started��m_buffer��ύX�����ɒ�~����B�j
		public void pause()
		{
			if (m_node != null) m_node.Stop();
		}
		//�ꎞ��~�̉����im_started��m_buffer��ύX�����Đ�����B�j
		public void pauseDefeat()
		{
			if (m_node != null) m_node.Start();
		}
		//�I���i�����j
//		public void Close()//��O�����̂��ƂɂȂ�̂Ŏg�p���Ȃ��I�I
//		{
//			if (m_node != null)
//			{
//				m_node.Dispose();
//			}
//			m_started = false;
//			flag = false;
//		}
		//true�̎��ɊJ�n����Ȃǁi�O�����玩�R�Ɏg���j
		public bool flag;

        public void Dispose()//��O�����̂��ƂɂȂ�̂Ŏg�p���Ȃ��I�I
        {
        	if (m_node != null)
        	{
				m_node.Stop();
 //       		m_node.Dispose();
        	}
        	m_started = false;
        	flag = false;
        }

        //�R���X�g���N�^
        private void makeCAudioFileInputNode(
			Windows.Media.Audio.AudioGraph graph, //AudioGraph�I�u�W�F�N�g�ւ̎Q��
			Windows.Media.Audio.AudioDeviceOutputNode outputNode,//OutputNode�ւ̎Q��
			System.Exception pHr,//�G���[�ԍ�
			string logName,//���������O�t�@�C����
            string name,//�����t�@�C���̃t�@�C����
			System.Nullable<int> LoopCount = null//���[�v�J�E���g�i1��̏ꍇ��0�C2��̏ꍇ��1�C�c�Cnull�͖������[�v
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
			string name,//�����f�[�^�̃t�@�C����
			Windows.Media.Audio.AudioGraph graph,
			Windows.Media.Audio.AudioDeviceOutputNode outputNode,
			System.Nullable<int> LoopCount = null,
			string logName = null//���������O�t�@�C����
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
					if (fResult.Status == Windows.Media.Audio.AudioFileNodeCreationStatus.Success)//�쐬����
					{
						fResult.FileInputNode.Stop();
						fResult.FileInputNode.AddOutgoingConnection(outputNode);
						fResult.FileInputNode.LoopCount = LoopCount;
						m_node = fResult.FileInputNode;
						if (LoopCount == null)//���[�v�Đ���
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
						await fp1.WriteLineAsync("AudioFileInputNode�쐬���s");
						await fp1.WriteLineAsync(hr.ToString());
						fp1.Close();
					}
				}
                if (logName != null)
                {
                    System.IO.StreamWriter fp1 = new System.IO.StreamWriter(logName, true);
                    await fp1.WriteLineAsync("AudioFileInputNode�̍쐬����");
                    await fp1.WriteLineAsync("�t�@�C�����F" + name_normal);
                    fp1.Close();
                }
            });
			th.Start();
			if(th.ThreadState != System.Threading.ThreadState.Unstarted) th.Join();

            if (result != null) return result.ExtendedError;
			else return new System.Exception("AudioFileInputNode�쐬���s");
		}

    }
}