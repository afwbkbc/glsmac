#pragma once

#include "base/Base.h"

#define SDL_MAIN_HANDLED 1
#include <SDL2/SDL.h>

#include "scene/actor/SoundActor.h"

namespace audio {
namespace sdl2 {

class SDL2Audio;
	
MAJOR_CLASS( SoundActor, base::Base )

	SoundActor( scene::actor::SoundActor* actor );
	~SoundActor();

	const bool IsActive();
	const float GetVolume() const;
	void GetNextBuffer( uint8_t* buffer, int len );
	
private:
	scene::actor::SoundActor *m_actor = nullptr;
	
};

}
}
