#pragma once

#include "Actor.h"

#include "types/Sound.h"
#include "util/Timer.h"

using namespace std;
using namespace types;

namespace scene {
namespace actor {

class SoundActor : public Actor {
public:
	SoundActor( const std::string &name, const Sound *sound );
	~SoundActor();

	const unsigned char* GetBuffer( const size_t pos, const size_t len );
	
	const Sound* GetSound() const;

	void SetRepeatable( const bool repeatable );
	void SetStartDelay( const size_t start_delay );
	void SetAutoPlay( const bool autoplay );
	void SetVolume( const float volume );
	
	void Play();
	void Pause();
	void Stop();
	
	const bool IsActive();
	const float GetVolume() const;
	const bool IsReadyToBeDeleted() const;
	
	void Rewind();
	void GetNextBuffer( uint8_t* buffer, int len );
private:
	const Sound* m_sound = nullptr;
	
	bool m_is_repeatable = false;
	
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
