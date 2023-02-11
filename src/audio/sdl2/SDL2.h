#pragma once

#include <unordered_map>

#define SDL_MAIN_HANDLED 1
#include <SDL_audio.h>

#include <mutex>

#include "scene/actor/Sound.h"
#include "Sound.h"

#include "../Audio.h"

// this must match specs of SMAC wavs until resampling is implemented
#define AUDIO_FREQUENCY 22050
#define AUDIO_FORMAT AUDIO_S16
#define AUDIO_CHANNELS 1
#define AUDIO_SAMPLE_TYPE int16_t
#define AUDIO_MIX_TYPE double
#define AUDIO_SAMPLES 4096
#define AUDIO_VOLUME 0.7 // TODO: put to config/settings
#define AUDIO_VOLUME_LOWERING_FROM_ACTORS 0.95 // helps with clicks a bit. it's used as pow( AUDIO_VOLUME_LOWERING, number_of_active_channels )

namespace audio {
namespace sdl2 {

CLASS( SDL2, Audio )
	SDL2();
	~SDL2();
	
	void Start();
	void Stop();
	void Iterate();
	
	void AddActor( scene::actor::Sound *actor );
	void RemoveActor( scene::actor::Sound *actor );
	
	void Mix( Uint8 *stream, int len );
	
protected:
	
private:
	bool m_is_sound_enabled = false;
	
	mutex m_actors_mutex;
	unordered_map< scene::actor::Sound*, sdl2::Sound* > m_actors = {};
	
	size_t m_buffer_length = 0;
	size_t m_buffer_size = 0;
	size_t m_mix_buffer_size = 0;
	AUDIO_MIX_TYPE* m_mix_buffer = nullptr;
	AUDIO_SAMPLE_TYPE* m_buffer = nullptr;
};

} /* namespace sdl2 */
} /* namespace audio */
