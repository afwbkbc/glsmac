#pragma once

#define SDL_MAIN_HANDLED 1
#include <SDL.h>

#include "common/Common.h"

namespace scene::actor {
class Sound;
}

namespace audio {
namespace sdl2 {

class SDL2;

CLASS( Sound, common::Class )

	Sound( scene::actor::Sound* actor );
	~Sound();

	const bool IsActive();
	const float GetVolume() const;
	void GetNextBuffer( uint8_t* buffer, int len );

private:
	scene::actor::Sound* m_actor = nullptr;

};

}
}
