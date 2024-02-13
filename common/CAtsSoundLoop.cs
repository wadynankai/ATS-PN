
namespace AtsPlugin
{
	internal class CAtsSoundLoop
	{
		private int index { get; }
		public CAtsSoundLoop(int _index)
		{
			index = _index;
		}
		unsafe public void Run(int* __p_sound)
		{
			if (m_firstTime)
			{
				__p_sound[index] = ats_sound_stop;
				m_firstTime = false;
			}
			else
			{
				if (m_play)
				{
					__p_sound[index] = m_vol;
				}
				else __p_sound[index] = ats_sound_stop;
			}
		}
		public bool isRunning()
		{
			return m_play;
		}
		public void Start()
		{
			m_play = true;
		}
		public void Stop()
		{
			m_play = false;
		}
		public void SetVolume(float vol)
		{
//			var cof = CURRENT_SET.linear(0.0f, ats_sound_stop, 1.0f, ats_sound_playlooping);
//			m_vol = (int)(cof.first * vol + cof.second);
//			if (m_vol > ats_sound_playlooping) m_vol = ats_sound_playlooping;
//			if (m_vol < ats_sound_stop) m_vol = ats_sound_stop;
		}
		public static implicit operator int(CAtsSoundLoop s) => s.index;

		const int ats_sound_stop = -10000;
		const int ats_sound_playlooping = 0;
		private int m_vol = ats_sound_playlooping;
		private bool m_play = true;
		private bool m_firstTime = true;
	}
}
