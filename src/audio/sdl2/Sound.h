#pragma once

#include "base/Base.h"

#define SDL_MAIN_HANDLED 1
#include <SDL2/SDL.h>

#include "scene/actor/Sound.h"

namespace audio {
namespace sdl2 {

class SDL2;
	
CLASS( Sound, base::Base )

	Sound( scene::actor::Sound* actor );
	~Sound();

	const bool IsActive();
	const float GetVolume() const;
	void GetNextBuffer( uint8_t* buffer, int len );
	
private:
	scene::actor::Sound *m_actor = nullptr;
	
};

}
}
