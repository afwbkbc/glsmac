#pragma once

#include "Actor.h"

#include "util/Timer.h"

namespace types {
class Sound;
}

namespace scene {
namespace actor {

CLASS( Sound, Actor )

	Sound( const std::string& name, const types::Sound* sound );
	~Sound();

	const types::Sound* GetSound() const;

	void SetRepeatable( const bool repeatable );
	void SetStartDelay( const size_t start_delay );
	void SetAutoPlay( const bool autoplay );
	void SetVolume( const float volume );

	void Play();
	void Pause();
	void Stop();

	void Mute();

	const bool IsActive();
	const bool IsFinished() const;
	const float GetVolume() const;
	const bool IsReadyToBeDeleted() const;

	const size_t GetPos() const;

	void Rewind();
	void GetNextBuffer( uint8_t* buffer, int len );

private:
	const types::Sound* m_sound = nullptr;

	bool m_is_repeatable = false;
	bool m_is_muted = false;
	bool m_is_playing = false;
	bool m_is_autoplay = false;

	bool m_is_active = false;
	util::Timer m_start_delay_timer;
	size_t m_start_delay = 0;

	float m_volume = 1.0;

	bool m_is_finished = false;
	size_t m_pos = 0;
};

}
}
