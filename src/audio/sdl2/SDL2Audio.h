#pragma once

#include <unordered_map>

#define SDL_MAIN_HANDLED 1
#include <SDL2/SDL_audio.h>

#include "scene/actor/SoundActor.h"

#include "../Audio.h"

#include <mutex>

#include "SoundActor.h"

namespace audio {
namespace sdl2 {

MINOR_CLASS( SDL2, Audio )
	SDL2Audio();
	~SDL2Audio();
	
	void Start();
	void Stop();
	void Iterate();
	
	void AddActor( scene::actor::SoundActor *actor );
	void RemoveActor( scene::actor::SoundActor *actor );
	
	void Mix( Uint8 *stream, int len );
	
protected:
	
private:
	bool m_is_sound_enabled = false;
	
	mutex m_actors_mutex;
	unordered_map< scene::actor::SoundActor*, SoundActor* > m_actors = {};
	
};

} /* namespace sdl2 */
} /* namespace audio */
