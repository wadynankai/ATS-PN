#ifndef CATS_SOUND_INCLUDED
#define CATS_SOUND_INCLUDED

template <size_t index = static_cast<size_t>(-1)>class CAtsSound
{
public:
	void operator()(int* __p_sound)noexcept
	{
		static_assert(std::cmp_less(index, 256), "Index must be 0-255.");
		if constexpr (std::cmp_less(index, 256))
		{
			if (FirstTime)
			{
				__p_sound[index] = ats_sound_stop;
				FirstTime = false;
			}
			else [[likely]]
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
				else [[likely]]
				{
					__p_sound[index] = ats_sound_continue;
				}
			}
		}
	}
	constexpr void Start()noexcept
	{
		play = true;
		stop = false;
	}
	constexpr void Stop()noexcept
	{
		play = false;
		stop = true;
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
	static constexpr int ats_sound_play = 1;
	static constexpr int ats_sound_continue = 2;
	bool play = false;
	bool stop = false;
	bool FirstTime = true;
};

#endif // !CATS_SOUND_INCLUDED