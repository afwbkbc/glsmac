#define SDL_MAIN_HANDLED 1

#include <SDL.h>

#include "SDL2.h"

#include "util/FS.h"
#include "types/Sound.h"

namespace loader {
namespace sound {

SDL2::~SDL2() {
	for ( auto& sound : m_sounds ) {
		DELETE( sound.second );
	}
}

types::Sound* SDL2::LoadSoundImpl( const std::string& filename ) {

	sound_map_t::iterator it = m_sounds.find( filename );
	if ( it != m_sounds.end() ) {
		return it->second;
	}
	else {

		Log( "Loading sound \"" + filename + "\"" );

		Uint8* wav_buffer = nullptr; // buffer containing our audio file
		Uint32 wav_length = 0; // length of our sample
		SDL_AudioSpec wav_spec; // the specs of our piece of music

		SDL_AudioSpec* ret = nullptr;

		/* Load the WAV */
		// the specs, length and buffer of our wav are filled
		ret = SDL_LoadWAV( filename.c_str(), &wav_spec, &wav_buffer, &wav_length );
		if ( !ret ) {
			return nullptr;
		}

		NEWV( sound, types::Sound );
		sound->m_name = filename;

		sound->m_buffer_size = wav_length;
		sound->m_buffer = (unsigned char*)malloc( sound->m_buffer_size );
		memcpy( ptr( sound->m_buffer, 0, wav_length ), wav_buffer, wav_length );

		sound->m_spec.channels = wav_spec.channels;
		sound->m_spec.format = wav_spec.format;
		sound->m_spec.freq = wav_spec.freq;
		sound->m_spec.padding = wav_spec.padding;
		sound->m_spec.samples = wav_spec.samples;
		sound->m_spec.silence = wav_spec.silence;
		sound->m_spec.size = wav_spec.size;

		SDL_FreeWAV( wav_buffer );

		m_sounds[ filename ] = sound;

		return sound;
	}
}

}
}
