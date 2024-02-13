#ifndef CATS_SOUND_LOOP_INCLUDED
#define CATS_SOUND_LOOP_INCLUDED
#include "CURRENT_SET.h"

template <size_t index = static_cast<size_t>(-1)>class CAtsSoundLoop
{
public:
	void operator()(int* __p_sound) noexcept
	{
		static_assert(std::cmp_less(index, 256), "Index must be 0-255.");
		if constexpr (std::cmp_less(index,256))
		{
			if (FirstTime)
			{
				__p_sound[index] = ats_sound_stop;
				FirstTime = false;
			}
			else[[likely]]
			{
				if (m_play)
				{
					__p_sound[index] = m_vol;
				}
				else __p_sound[index] = ats_sound_stop;
			}
		}
	}
	_NODISCARD constexpr const bool isRunning()const noexcept
	{
		return m_play;
	}
	constexpr void Start()noexcept
	{
		m_play = true;
	}
	constexpr void Stop()noexcept
	{
		m_play = false;
	}
	constexpr void SetVolume(float vol)noexcept
	{
		constexpr auto cof = linear<float, float>(0.0f, ats_sound_stop, 1.0f, ats_sound_playlooping);
		m_vol = static_cast<int>(cof.first * vol + cof.second);
		if (m_vol > ats_sound_playlooping)m_vol = ats_sound_playlooping;
		if (m_vol < ats_sound_stop)m_vol = ats_sound_stop;
	}
	_NODISCARD constexpr size_t getIndex()const noexcept
	{
		static_assert(std::cmp_less(index, 256), "Index must be 0-255.");
		return index;
	}
	_NODISCARD constexpr operator size_t()const noexcept
	{
		static_assert(std::cmp_less(index, 256), "Index must be 0-255.");
		return index;
	}
private:
	static constexpr int ats_sound_stop = -10000;
	static constexpr int ats_sound_playlooping = 0;
	int m_vol = ats_sound_playlooping;
	bool m_play = true;
	bool FirstTime = true;
};

#endif // !CATS_SOUND_LOOP_INCLUDED