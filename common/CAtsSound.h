#ifndef CATS_SOUND_INCLUDED
#define CATS_SOUND_INCLUDED

template <size_t index = static_cast<size_t>(-1)>class CAtsSound
{
public:
	_NODISCARD const int operator()()noexcept
	{
		if (play)
		{
			play = false;
			return ats_sound_play;
		}
		else if (stop)
		{
			stop = false;
			return ats_sound_stop;
		}
		else [[likely]]
		{
			return ats_sound_continue;
		}
	}
	void Start()noexcept
	{
		play = true;
		stop = false;
	}
	void Stop()noexcept
	{
		play = false;
		stop = true;
	}
	_NODISCARD constexpr size_t getIndex()const noexcept
	{
		static_assert(index < 256, "Index must be 0-255.");
		return index;
	}
	_NODISCARD constexpr operator size_t()const noexcept
	{
		static_assert(index < 256, "Index must be 0-255.");
		return index;
	}
private:
	static constexpr int ats_sound_stop = -10000;
	static constexpr int ats_sound_play = 1;
	static constexpr int ats_sound_continue = 2;
	bool play = false;
	bool stop = false;
};

#endif // !CATS_SOUND_INCLUDED