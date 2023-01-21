#pragma once

#include <unordered_map>

#define SDL_MAIN_HANDLED 1
#include <SDL2/SDL_audio.h>

#include "scene/actor/SoundActor.h"

#include "../Audio.h"

#include <mutex>

#include "SoundActor.h"

// this must match specs of SMAC wavs until resampling is implemented
#define AUDIO_FREQUENCY 22050
#define AUDIO_FORMAT AUDIO_S16
#define AUDIO_CHANNELS 1
#define AUDIO_SAMPLE_TYPE int16_t
#define AUDIO_MIX_TYPE double
#define AUDIO_SAMPLES 4096
#define AUDIO_VOLUME_LOWERING 0.95 // helps with clicks a bit. it's used as pow( AUDIO_VOLUME_LOWERING, number_of_active_channels )

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
	
	size_t m_buffer_length = 0;
	size_t m_buffer_size = 0;
	size_t m_mix_buffer_size = 0;
	AUDIO_MIX_TYPE* m_mix_buffer = nullptr;
	AUDIO_SAMPLE_TYPE* m_buffer = nullptr;
};

} /* namespace sdl2 */
} /* namespace audio */
