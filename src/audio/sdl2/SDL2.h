#pragma once

#include <unordered_map>

#define SDL_MAIN_HANDLED 1

#include <SDL_audio.h>

#include <mutex>

#include "../Audio.h"

// this must match specs of SMAC wavs until resampling is implemented
#define AUDIO_FREQUENCY 22050
#define AUDIO_FORMAT AUDIO_S16
#define AUDIO_CHANNELS 1
#define AUDIO_SAMPLE_TYPE int16_t
#define AUDIO_MIX_TYPE double
#define AUDIO_SAMPLES 4096
#define AUDIO_VOLUME 0.75 // TODO: put to config/settings
#define AUDIO_VOLUME_LOWERING_FROM_ACTORS 0.92 // helps with clicks a bit. it's used as pow( x, number_of_active_channels )

namespace audio {
namespace sdl2 {

class Sound;

CLASS( SDL2, Audio )
	SDL2();
	~SDL2();

	void Start() override;
	void Stop() override;
	void Iterate() override;

	void AddActor( scene::actor::Sound* actor ) override;
	void RemoveActor( scene::actor::Sound* actor ) override;

	void Mix( Uint8* stream, int len );

protected:

private:
	bool m_is_sound_enabled = false;

	std::mutex m_actors_mutex;
	std::unordered_map< scene::actor::Sound*, sdl2::Sound* > m_actors = {};

	size_t m_buffer_length = 0;
	size_t m_buffer_size = 0;
	size_t m_mix_buffer_size = 0;
	AUDIO_MIX_TYPE* m_mix_buffer = nullptr;
	AUDIO_SAMPLE_TYPE* m_buffer = nullptr;
};

}
}
