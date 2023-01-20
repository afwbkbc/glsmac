#include "SDL2Audio.h"

#include "engine/Engine.h"

#include <algorithm>

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

SDL2Audio::SDL2Audio() {
	
}

SDL2Audio::~SDL2Audio() {
	lock_guard<mutex> guard( m_actors_mutex );
	for ( auto& actor : m_actors ) {
		DELETE( actor.second );
	}
}

void SDL2Audio_callback( void* userdata, Uint8* stream, int len ) {
	auto audio = (SDL2Audio*)userdata;
	audio->Mix( stream, len );
}

void SDL2Audio::Start() {
	Log( "Initializing SDL2" );
	
	if ( SDL_Init( SDL_INIT_AUDIO ) ) {
		Log( "failed to enable audio, game will start without sound" );
		return;
	}
	
	SDL_AudioSpec wav_spec;
    wav_spec.freq = AUDIO_FREQUENCY;
    wav_spec.format = AUDIO_FORMAT;
    wav_spec.channels = AUDIO_CHANNELS;
    wav_spec.samples = AUDIO_SAMPLES;
    wav_spec.callback = SDL2Audio_callback;
    wav_spec.userdata = this;
	
	/* Open the audio device */
	auto ret = SDL_OpenAudio( &wav_spec, NULL );
	ASSERT( ret >= 0, (string) "Couldn't open audio: " + SDL_GetError() );
	
	/* Start playing */
	SDL_PauseAudio(0);

	m_is_sound_enabled = true;
}

void SDL2Audio::Stop() {
	if ( !m_is_sound_enabled ) {
		return;
	}
	
	Log( "Deinitializing SDL2" );
	SDL_CloseAudio();
	
}

void SDL2Audio::Iterate() {
	if ( !m_is_sound_enabled ) {
		return;
	}
	
	Audio::Iterate();
	
}

void SDL2Audio::AddActor( scene::actor::SoundActor *actor ) {
	if ( !m_is_sound_enabled ) {
		return;
	}
	
	lock_guard<mutex> guard( m_actors_mutex );
	
	Log( "Adding sound actor " + actor->GetName() );
	ASSERT( m_actors.find( actor ) == m_actors.end(), "sound actor already added" );
	NEWV( sound_actor, SoundActor, actor );
	m_actors[ actor ] = sound_actor;
}

void SDL2Audio::RemoveActor( scene::actor::SoundActor *actor ) {
	if ( !m_is_sound_enabled ) {
		return;
	}
	
	lock_guard<mutex> guard( m_actors_mutex );
	
	Log( "Removing sound actor " + actor->GetName() );
	auto it = m_actors.find( actor );
	ASSERT( it != m_actors.end(), "sound actor not found" );
	DELETE( it->second );
	m_actors.erase( it );
}


void SDL2Audio::Mix( Uint8* stream, int len ) {
	
	size_t buffer_length = AUDIO_SAMPLES * AUDIO_CHANNELS;
	size_t buffer_size = sizeof(AUDIO_SAMPLE_TYPE) * buffer_length;
	size_t mix_buffer_size = sizeof(AUDIO_MIX_TYPE) * buffer_length;
	
	ASSERT( len == buffer_size, "sample type or size mismatch" );
	
	AUDIO_SAMPLE_TYPE* buffer = (AUDIO_SAMPLE_TYPE*)malloc( buffer_size );
	
	AUDIO_MIX_TYPE* mix_buffer = (AUDIO_MIX_TYPE*)malloc( mix_buffer_size );
	memset( ptr( mix_buffer, 0, mix_buffer_size ), 0, mix_buffer_size );
	
	{
		lock_guard<mutex> guard( m_actors_mutex );
		
		for (auto& actor : m_actors ) {
			if ( actor.second->IsActive() ) {
				actor.second->GetNextBuffer( (uint8_t*)buffer, len );
				for ( size_t i = 0 ; i < buffer_length ; i++ ) {
					mix_buffer[i] = mix_buffer[i] + buffer[i] * actor.second->GetVolume() * pow( AUDIO_VOLUME_LOWERING, m_actors.size() );
				}
			}
		}
	}
	for ( size_t i = 0 ; i < buffer_length ; i++ ) {
		buffer[i] = floor(mix_buffer[i]);
	}

	SDL_memcpy( stream, (uint8_t*)buffer, len );
	
	free( buffer );
	free( mix_buffer );
}

}
}
