#include "Sound.h"

#include "types/Sound.h"
#include "scene/actor/Sound.h"

#include "engine/Engine.h"
#include "audio/Audio.h"
#include "loader/sound/SoundLoader.h"

#include "gse/value/Bool.h"
#include "gse/value/Float.h"

namespace ui {
namespace dom {

Sound::Sound( DOM_ARGS )
	: Object( DOM_ARGS_PASS, "surface" ) {

	Iterable(
		[ this ]() {
			std::vector< scene::actor::Sound* > active_actors = {};
			for ( auto& actor : m_actors ) {
				if ( actor->IsFinished() ) {
					g_engine->GetAudio()->RemoveAndDeleteActor( actor );
				}
				else {
					active_actors.push_back( actor );
				}
			}
			m_actors = active_actors;
		}
	);

	Property(
		GSE_CALL, "sound", gse::Value::T_STRING, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			const auto& value = ( (gse::value::String*)v )->value;
			auto* sound = g_engine->GetSoundLoader()->LoadCustomSound( value );
			SetSound( sound );
			if ( m_autoplay ) {
				Play();
			}
		},
		[ this ]( GSE_CALLABLE ) {
			SetSound( nullptr );
		}
	);

	Property(
		GSE_CALL, "autoplay", gse::Value::T_BOOL, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			SetAutoPlay( ( (gse::value::Bool*)v )->value );
		},
		[ this ]( GSE_CALLABLE ) {
			SetAutoPlay( false );
		}
	);

	Property(
		GSE_CALL, "repeat", gse::Value::T_BOOL, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			SetRepeatable( ( (gse::value::Bool*)v )->value );
		},
		[ this ]( GSE_CALLABLE ) {
			SetRepeatable( false );
		}
	);

	Property(
		GSE_CALL, "volume", gse::Value::T_FLOAT, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			SetVolume( ( (gse::value::Float*)v )->value );
		},
		[ this ]( GSE_CALLABLE ) {
			SetVolume( 1.0f );
		}
	);

	Property(
		GSE_CALL, "delay", gse::Value::T_INT, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			SetStartDelay( ( (gse::value::Int*)v )->value );
		},
		[ this ]( GSE_CALLABLE ) {
			SetStartDelay( 0 );
		}
	);

	Method( GSE_CALL, "play", NATIVE_CALL( this ) {
		Play();
		return nullptr;
	} ) );

	Method( GSE_CALL, "stop", NATIVE_CALL( this ) {
		Stop();
		return nullptr;
	} ) );

}

Sound::~Sound() {
	for ( auto& actor : m_actors ) {
		actor->SetRepeatable( false );
		actor->Stop();
		g_engine->GetAudio()->RemoveAndDeleteActor( actor );
	}
}

void Sound::SetSound( const types::Sound* sound ) {
	if ( sound != m_sound ) {
		if ( m_sound ) {
			Stop();
		}
		m_sound = sound;
	}
}

void Sound::SetAutoPlay( const bool autoplay ) {
	if ( autoplay != m_autoplay ) {
		m_autoplay = autoplay;
		for ( auto& actor : m_actors ) {
			actor->SetAutoPlay( autoplay );
		}
	}
}

void Sound::SetAutoStop( const bool autostop ) {
	if ( autostop != m_autostop ) {
		m_autostop = autostop;
	}
}

void Sound::SetRepeatable( const bool repeatable ) {
	if ( repeatable != m_repeatable ) {
		m_repeatable = repeatable;
		for ( auto& actor : m_actors ) {
			actor->SetRepeatable( m_repeatable );
		}
	}
}

void Sound::SetStartDelay( const size_t start_delay ) {
	if ( start_delay != m_start_delay ) {
		m_start_delay = start_delay;
		for ( auto& actor : m_actors ) {
			actor->SetStartDelay( m_start_delay );
		}
	}
}

void Sound::SetVolume( const float volume ) {
	if ( volume != m_volume ) {
		m_volume = volume;
		for ( auto& actor : m_actors ) {
			actor->SetVolume( m_volume );
		}
	}
}

void Sound::Play() {
	if ( m_sound ) {
		NEWV( actor, scene::actor::Sound, "UI::SoundEffect", m_sound );
		actor->SetRepeatable( m_repeatable );
		actor->SetStartDelay( m_start_delay );
		actor->SetVolume( m_volume );
		actor->SetAutoPlay( m_autoplay );
		g_engine->GetAudio()->AddActor( actor );
		m_actors.push_back( actor );
		actor->Play();
	}
}

void Sound::Pause() {
	for ( auto& actor : m_actors ) {
		actor->Pause();
	}
}

void Sound::Stop() {
	for ( auto& actor : m_actors ) {
		actor->SetRepeatable( false );
		actor->Stop();
	}
}

}
}
