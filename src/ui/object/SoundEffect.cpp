#include "SoundEffect.h"
#include "engine/Engine.h"

namespace ui {
namespace object {

SoundEffect::SoundEffect( const std::string& class_name ) : UIObject( class_name )
{
	
}

void SoundEffect::SetSound( const types::Sound* sound ) {
	if ( sound != m_sound ) {
		m_sound = sound;
	}
}

void SoundEffect::SetAutoPlay( const bool autoplay ) {
	if ( autoplay != m_autoplay ) {
		m_autoplay = autoplay;
		for ( auto& actor : m_actors ) {
			actor->SetAutoPlay( autoplay );
		}
	}
}

void SoundEffect::SetRepeatable( const bool repeatable ) {
	if ( repeatable != m_repeatable ) {
		m_repeatable = repeatable;
		for ( auto& actor : m_actors ) {
			actor->SetRepeatable( m_repeatable );
		}
	}
}

void SoundEffect::SetStartDelay( const size_t start_delay ) {
	if ( start_delay != m_start_delay ) {
		m_start_delay = start_delay;
		for ( auto& actor : m_actors ) {
			actor->SetStartDelay( m_start_delay );
		}
	}
}

void SoundEffect::SetVolume( const float volume ) {
	if ( volume != m_volume ) {
		m_volume = volume;
		for ( auto& actor : m_actors ) {
			actor->SetVolume( m_volume );
		}
	}
}

void SoundEffect::Play() {
	if ( m_sound && m_created ) {
		//Log( "Creating sound actor" );
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

void SoundEffect::Pause() {
	for ( auto& actor : m_actors ) {
		actor->Pause();
	}
}

void SoundEffect::Stop() {
	for ( auto& actor : m_actors ) {
		//actor->Stop();
	}
}

void SoundEffect::Create() {
	UIObject::Create();
	
	if ( m_autoplay && m_actors.empty() ) {
		Play();
	}
}

void SoundEffect::Iterate() {
	UIObject::Iterate();
	
	std::vector< actor::Sound* > active_actors = {};
	for ( auto& actor : m_actors ) {
		if ( actor->IsFinished() ) {
			//Log( "Destroying inactive sound actor" );
			g_engine->GetAudio()->RemoveAndDeleteActor( actor );
		}
		else {
			active_actors.push_back( actor );
		}
	}
	m_actors = active_actors;
}

void SoundEffect::Destroy() {
	for ( auto& actor : m_actors ) {
		//actor->Stop(); // stop or not?
		g_engine->GetAudio()->RemoveAndDeleteActor( actor );
		// TODO: scene?
		//RemoveActor( m_actor );
	}
	m_actors.clear();
	
	UIObject::Destroy();
}

void SoundEffect::ApplyStyle() {
	UIObject::ApplyStyle();
	
	if ( Has( Style::A_SOUND ) ) {
		const auto* sound = (types::Sound*)GetObject( Style::A_SOUND );
		if ( sound ) {
			SetSound( sound );
		}
		if ( Has( Style::A_SOUND_VOLUME ) ) {
			SetVolume( Get( Style::A_SOUND_VOLUME ) );
		}
		if ( Has( Style::A_SOUND_AUTOPLAY ) ) {
			SetAutoPlay( true );
		}
		if ( Has( Style::A_SOUND_REPEATABLE ) ) {
			SetRepeatable( true );
		}
		if ( Has( Style::A_SOUND_START_DELAY ) ) {
			SetStartDelay( Get( Style::A_SOUND_START_DELAY ) );
		}
		
		if ( m_autoplay && m_actors.empty() ) {
			Play();
		}
	}
	
}

}
}
