#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

#define SDL_MAIN_HANDLED 1
#include <SDL.h>
#ifdef __APPLE__
#include <pthread.h>
#endif

#include "SDL2.h"

#include "Sound.h"
#include "engine/Engine.h"
#include "config/Config.h"
#include "scene/actor/Sound.h"

namespace audio {
namespace sdl2 {

SDL2::SDL2() {

}

SDL2::~SDL2() {
	std::lock_guard guard( m_actors_mutex );
	for ( auto& actor : m_actors ) {
		DELETE( actor.second );
	}
}

#ifdef __APPLE__
void SDL2::InitSDLOnMainThread() {
	ASSERT( pthread_main_np() != 0, "InitSDLOnMainThread must be called from main thread" );
	// #region agent log
	{
		std::ofstream log("/Users/jhurliman/Documents/Code/jhurliman/glsmac/.cursor/debug.log", std::ios::app);
		auto tid = std::this_thread::get_id();
		std::stringstream ss;
		ss << tid;
		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		log << "{\"sessionId\":\"debug-session\",\"runId\":\"post-fix\",\"hypothesisId\":\"A\",\"location\":\"SDL2.cpp:InitSDLOnMainThread\",\"message\":\"SDL_Init(SDL_INIT_AUDIO) on main thread\",\"data\":{\"threadId\":\"" << ss.str() << "\",\"isMainThread\":" << (pthread_main_np() != 0 ? "true" : "false") << ",\"sdlWasInit\":\"" << SDL_WasInit(0) << "\"},\"timestamp\":" << now << "}\n";
	}
	// #endregion
	if ( SDL_Init( SDL_INIT_AUDIO ) ) {
		THROW( (std::string)"Failed to initialize SDL audio: " + SDL_GetError() );
	}
	// #region agent log
	{
		std::ofstream log("/Users/jhurliman/Documents/Code/jhurliman/glsmac/.cursor/debug.log", std::ios::app);
		auto tid = std::this_thread::get_id();
		std::stringstream ss;
		ss << tid;
		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		log << "{\"sessionId\":\"debug-session\",\"runId\":\"post-fix\",\"hypothesisId\":\"A\",\"location\":\"SDL2.cpp:InitSDLOnMainThread\",\"message\":\"SDL_Init(SDL_INIT_AUDIO) completed\",\"data\":{\"threadId\":\"" << ss.str() << "\",\"sdlWasInit\":\"" << SDL_WasInit(0) << "\"},\"timestamp\":" << now << "}\n";
	}
	// #endregion
}
#endif

void SDL2_callback( void* userdata, Uint8* stream, int len ) {
	auto audio = (SDL2*)userdata;
	audio->Mix( stream, len );
}

void SDL2::Start() {

	if ( g_engine->GetConfig()->HasLaunchFlag( config::Config::LF_NOSOUND ) ) {
		return;
	}

	Log( "Initializing SDL2 Audio" );

#ifdef __APPLE__
	// On macOS, SDL initialization must happen on the main thread due to AppKit requirements
	// SDL should already be initialized via InitSDLOnMainThread() called from main()
	ASSERT( SDL_WasInit( SDL_INIT_AUDIO ) != 0, "SDL audio not initialized - InitSDLOnMainThread() must be called from main thread before starting threads" );
	// #region agent log
	{
		std::ofstream log("/Users/jhurliman/Documents/Code/jhurliman/glsmac/.cursor/debug.log", std::ios::app);
		auto tid = std::this_thread::get_id();
		std::stringstream ss;
		ss << tid;
		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		log << "{\"sessionId\":\"debug-session\",\"runId\":\"post-fix\",\"hypothesisId\":\"A\",\"location\":\"SDL2.cpp:Start\",\"message\":\"SDL audio already initialized on main thread\",\"data\":{\"threadId\":\"" << ss.str() << "\",\"isMainThread\":" << (pthread_main_np() != 0 ? "true" : "false") << ",\"sdlWasInit\":\"" << SDL_WasInit(0) << "\"},\"timestamp\":" << now << "}\n";
	}
	// #endregion
#else
	// #region agent log
	{
		std::ofstream log("/Users/jhurliman/Documents/Code/jhurliman/glsmac/.cursor/debug.log", std::ios::app);
		auto tid = std::this_thread::get_id();
		std::stringstream ss;
		ss << tid;
		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		log << "{\"sessionId\":\"debug-session\",\"runId\":\"post-fix\",\"hypothesisId\":\"A\",\"location\":\"SDL2.cpp:40\",\"message\":\"SDL_Init(SDL_INIT_AUDIO) before call\",\"data\":{\"threadId\":\"" << ss.str() << "\",\"sdlWasInit\":\"" << SDL_WasInit(0) << "\"},\"timestamp\":" << now << "}\n";
	}
	// #endregion
	if ( SDL_Init( SDL_INIT_AUDIO ) ) {
		// #region agent log
		{
			std::ofstream log("/Users/jhurliman/Documents/Code/jhurliman/glsmac/.cursor/debug.log", std::ios::app);
			auto tid = std::this_thread::get_id();
			std::stringstream ss;
			ss << tid;
			auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			log << "{\"sessionId\":\"debug-session\",\"runId\":\"post-fix\",\"hypothesisId\":\"A\",\"location\":\"SDL2.cpp:41\",\"message\":\"SDL_Init(SDL_INIT_AUDIO) failed\",\"data\":{\"threadId\":\"" << ss.str() << "\",\"error\":\"" << SDL_GetError() << "\"},\"timestamp\":" << now << "}\n";
		}
		// #endregion
		Log( "Failed to enable audio, game will start without sound." );
		return;
	}
	// #region agent log
	{
		std::ofstream log("/Users/jhurliman/Documents/Code/jhurliman/glsmac/.cursor/debug.log", std::ios::app);
		auto tid = std::this_thread::get_id();
		std::stringstream ss;
		ss << tid;
		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		log << "{\"sessionId\":\"debug-session\",\"runId\":\"post-fix\",\"hypothesisId\":\"A\",\"location\":\"SDL2.cpp:44\",\"message\":\"SDL_Init(SDL_INIT_AUDIO) succeeded\",\"data\":{\"threadId\":\"" << ss.str() << "\",\"sdlWasInit\":\"" << SDL_WasInit(0) << "\"},\"timestamp\":" << now << "}\n";
	}
	// #endregion
#endif

	m_buffer_length = AUDIO_SAMPLES * AUDIO_CHANNELS;
	m_buffer_size = sizeof( AUDIO_SAMPLE_TYPE ) * m_buffer_length;
	m_mix_buffer_size = sizeof( AUDIO_MIX_TYPE ) * m_buffer_length;
	m_mix_buffer = (AUDIO_MIX_TYPE*)malloc( m_mix_buffer_size );
	m_buffer = (AUDIO_SAMPLE_TYPE*)malloc( m_buffer_size );

	SDL_AudioSpec wav_spec;
	wav_spec.freq = AUDIO_FREQUENCY;
	wav_spec.format = AUDIO_FORMAT;
	wav_spec.channels = AUDIO_CHANNELS;
	wav_spec.samples = AUDIO_SAMPLES;
	wav_spec.callback = SDL2_callback;
	wav_spec.userdata = this;

	/* Open the audio device */
	auto ret = SDL_OpenAudio( &wav_spec, NULL );
	if ( ret < 0 ) {
		Log( (std::string)"Couldn't open audio: " + SDL_GetError() );
		Log( "Failed to enable audio, game will start without sound." );
		return;
	}

	m_is_sound_enabled = true;

	/* Start playing */
	SDL_PauseAudio( 0 );
}

void SDL2::Stop() {
	if ( !m_is_sound_enabled ) {
		return;
	}

	Log( "Deinitializing SDL2" );
	SDL_CloseAudio();

	free( m_buffer );
	free( m_mix_buffer );

}

void SDL2::Iterate() {
	if ( !m_is_sound_enabled ) {
		return;
	}

	Audio::Iterate();
}

void SDL2::AddActor( scene::actor::Sound* actor ) {
	if ( !m_is_sound_enabled ) {
		return;
	}

	std::lock_guard guard( m_actors_mutex );

	// check if same sound was double-added (can happen if two popups close at same time, for example)
	// in this case we'll ignore second sound to avoid volume spike
	for ( const auto& a : m_actors ) {
		if ( a.first->GetPos() == 0 && a.first->GetSound() == actor->GetSound() ) {
			Log( "Muting " + actor->GetName() + " to prevent double-sound" );
			actor->Mute();
		}
	}

	Log( "Adding sound actor " + actor->GetName() );
	ASSERT( m_actors.find( actor ) == m_actors.end(), "sound actor already added" );
	NEWV( sound_actor, Sound, actor );
	m_actors[ actor ] = sound_actor;
}

void SDL2::RemoveActor( scene::actor::Sound* actor ) {
	if ( !m_is_sound_enabled ) {
		return;
	}

	std::lock_guard guard( m_actors_mutex );

	Log( "Removing sound actor " + actor->GetName() );
	auto it = m_actors.find( actor );
	ASSERT( it != m_actors.end(), "sound actor not found" );
	DELETE( it->second );
	m_actors.erase( it );
}

void SDL2::Mix( Uint8* stream, int len ) {
	ASSERT( m_is_sound_enabled, "SDL2::Mix() called while sound not enabled" );
	ASSERT( len == m_buffer_size, "sample type or size mismatch" );

	memset( ptr( m_mix_buffer, 0, m_mix_buffer_size ), 0, m_mix_buffer_size );
	{
		std::lock_guard guard( m_actors_mutex );

		for ( auto& actor : m_actors ) {
			if ( actor.second->IsActive() ) {
				actor.second->GetNextBuffer( (uint8_t*)m_buffer, len );
				for ( size_t i = 0 ; i < m_buffer_length ; i++ ) {
					m_mix_buffer[ i ] = m_mix_buffer[ i ] + m_buffer[ i ] * actor.second->GetVolume() * AUDIO_VOLUME * pow( AUDIO_VOLUME_LOWERING_FROM_ACTORS, m_actors.size() );
				}
			}
		}
	}
	for ( size_t i = 0 ; i < m_buffer_length ; i++ ) {
		m_buffer[ i ] = floor( m_mix_buffer[ i ] );
	}

	SDL_memcpy( stream, (uint8_t*)m_buffer, len );
}

}
}
