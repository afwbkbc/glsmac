#include "SoundEffect.h"
#include "engine/Engine.h"

namespace ui {
namespace object {

SoundEffect::SoundEffect( const string& class_name ) : UIObject( class_name )
{
	
}

void SoundEffect::SetSound( const Sound* sound ) {
	if ( sound != m_sound ) {
		m_sound = sound;

		UpdateActor();
	}
}

void SoundEffect::SetAutoPlay( const bool autoplay ) {
	if ( autoplay != m_autoplay ) {
		m_autoplay = autoplay;
		if ( m_actor ) {
			m_actor->SetAutoPlay( autoplay );
		}
	}
}

void SoundEffect::SetRepeatable( const bool repeatable ) {
	if ( repeatable != m_repeatable ) {
		m_repeatable = repeatable;
		if ( m_actor ) {
			m_actor->SetRepeatable( m_repeatable );
		}
	}
}

void SoundEffect::SetStartDelay( const size_t start_delay ) {
	if ( start_delay != m_start_delay ) {
		m_start_delay = start_delay;
		if ( m_actor ) {
			m_actor->SetStartDelay( m_start_delay );
		}
	}
}

void SoundEffect::SetVolume( const float volume ) {
	if ( volume != m_volume ) {
		m_volume = volume;
		if ( m_actor ) {
			m_actor->SetVolume( m_volume );
		}
	}
}

void SoundEffect::Play() {
	if ( m_actor ) {
		m_actor->Play();
	}
}

void SoundEffect::Pause() {
	if ( m_actor ) {
		m_actor->Pause();
	}
}

void SoundEffect::Stop() {
	if ( m_actor ) {
		m_actor->Stop();
	}
}

void SoundEffect::Create() {
	UIObject::Create();
	
	UpdateActor();
}

void SoundEffect::UpdateActor() {
	if ( m_sound && m_created ) {
		if ( m_actor ) {
			g_engine->GetAudio()->RemoveAndDeleteActor( m_actor );
			// TODO: scene?
			//RemoveActor( m_actor );
		}
		NEW( m_actor, scene::actor::SoundActor, "UI::SoundEffect", m_sound );
			m_actor->SetRepeatable( m_repeatable );
			m_actor->SetStartDelay( m_start_delay );
			m_actor->SetVolume( m_volume );
		
		g_engine->GetAudio()->AddActor( m_actor );
		// TODO: scene?
		//AddActor( m_actor );
	}
}

void SoundEffect::Destroy() {
	if ( m_actor ) {
		
		g_engine->GetAudio()->RemoveAndDeleteActor( m_actor );
		// TODO: scene?
		//RemoveActor( m_actor );
		
		m_actor = nullptr;
	}
	
	UIObject::Destroy();
}

void SoundEffect::ApplyStyle() {
	UIObject::ApplyStyle();
	
	if ( Has( Style::A_SOUND ) ) {
		const auto* sound = (Sound*)GetObject( Style::A_SOUND );
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
	}
	
}

}
}
