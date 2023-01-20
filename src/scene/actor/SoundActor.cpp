#include "SoundActor.h"

#include <cstring>

namespace scene {
namespace actor {

SoundActor::SoundActor( const std::string &name, const Sound *sound )
	: Actor( TYPE_SOUND, name )
	, m_sound( sound )
{
	Rewind();
}

SoundActor::~SoundActor() {

}

const Sound* SoundActor::GetSound() const {
	return m_sound;
}

void SoundActor::Rewind() {
	m_pos = 0;
	m_is_finished = false;
	if ( m_is_autoplay ) {
		Play();
	}
}

void SoundActor::GetNextBuffer( uint8_t* buffer, int len ) {
	
	ASSERT( len < m_sound->m_buffer_size, "buffer size is smaller than len" );
	
	if ( m_is_finished || !m_is_active ) {
		memset( ptr( buffer, 0, len ), 0, len );
		return;
	}
	size_t newlen = 0;
	if ( len + m_pos > m_sound->m_buffer_size ) {
		newlen = m_sound->m_buffer_size - m_pos;
		//if ( !m_is_repeatable ) { // need to test this first (maybe not needed at all)
			memset( ptr( buffer, newlen, len - newlen ), 0, len - newlen );
			Stop();
		//}
		len = newlen;
	}
	memcpy( ptr( buffer, 0, len ), ptr( m_sound->m_buffer, m_pos, len ), len );
	m_pos += len;
	if ( m_is_finished && m_is_repeatable ) {
		Rewind();
		
		// need to test this first (maybe not needed at all)
		//GetNextBuffer( ptr( buffer, newlen, len - newlen ), newlen );
	}
}

void SoundActor::SetRepeatable( const bool repeatable ) {
	m_is_repeatable = repeatable;
	Rewind();
}

void SoundActor::SetStartDelay( const size_t start_delay ) {
	m_start_delay = start_delay;
	Rewind();
}

void SoundActor::SetAutoPlay( const bool autoplay ) {
	m_is_autoplay = autoplay;
	Rewind();
}

void SoundActor::SetVolume( const float volume ) {
	ASSERT( volume >= 0 && volume <= 1, "invalid volume " + to_string( volume ) );
	m_volume = volume;
}

void SoundActor::Play() {
	if ( !m_is_playing ) {
		ASSERT( !m_is_active, "not playing but active" );
		if ( m_is_finished ) {
			Rewind();
		}
		if ( m_start_delay > 0 ) {
			m_is_active = false;
			m_start_delay_timer.SetTimeout( m_start_delay );
		}
		else {
			m_is_active = true;
			m_is_playing = true;
		}
	}
}

void SoundActor::Pause() {
	if ( m_is_playing ) {
		m_is_playing = false;
		m_is_active = false;
	}
}

void SoundActor::Stop() {
	m_start_delay_timer.Stop();
	m_is_finished = true;
	m_is_playing = false;
	m_is_active = false;
	if ( m_is_repeatable ) {
		Rewind();
	}
}
	
const bool SoundActor::IsActive() {
	if ( !m_is_active ) {
		if ( m_start_delay_timer.Ticked() ) {
			m_is_active = true;
		}
	}
	return m_is_active;
}

const bool SoundActor::IsReadyToBeDeleted() const {
	return ( m_is_repeatable || !m_is_playing );
}

const float SoundActor::GetVolume() const {
	return m_volume;
}

}
}
