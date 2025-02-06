#pragma once

#include <vector>

#include "Area.h"

namespace types {
class Sound;
}

namespace scene::actor {
class Sound;
}

namespace ui {
namespace dom {

class Sound : public Object {
public:
	Sound( DOM_ARGS );

protected:
	virtual ~Sound();

private:

	const types::Sound* m_sound = nullptr;

	float m_volume = 1.0;
	bool m_autoplay = false;
	bool m_autostop = false;
	bool m_repeatable = false;
	size_t m_start_delay = 0;

	std::vector< scene::actor::Sound* > m_actors = {};

	void SetSound( const types::Sound* sound );
	void SetAutoPlay( const bool autoplay );
	void SetAutoStop( const bool autostop );
	void SetRepeatable( const bool repeatable );
	void SetStartDelay( const size_t start_delay );
	void SetVolume( const float volume );
	void Play();
	void Pause();
	void Stop();

};

}
}
