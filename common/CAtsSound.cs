using System;
using System.Collections.Generic;
using System.Text;

namespace AtsPlugin
{
	internal class CAtsSound
	{
		private int index { get; }

		public CAtsSound(int _index)
        {
			index = _index;
        }
		unsafe public void Run(int* __p_sound)
		{
			if (FirstTime)
			{
				__p_sound[index] = ats_sound_stop;
				FirstTime = false;
			}
			else
			{
				if (play)
				{
					play = false;
					__p_sound[index] = ats_sound_play;
				}
				else if (stop)
				{
					stop = false;
					__p_sound[index] = ats_sound_stop;
				}
				else
				{
					__p_sound[index] = ats_sound_continue;
				}
			}
		}
		public void Start()
		{
			play = true;
			stop = false;
		}
		public void Stop()
		{
			play = false;
			stop = true;
		}
		public static implicit operator int(CAtsSound s) => s.index;

		const int ats_sound_stop = -10000;
		const int ats_sound_play = 1;
		const int ats_sound_continue = 2;
		private bool play = false;
		private bool stop = false;
		private bool FirstTime = true;
    }
}
