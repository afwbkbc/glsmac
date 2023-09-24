#include <cstring>

#include "Sound.h"

namespace scene {
namespace actor {

Sound::Sound( const std::string& name, const types::Sound* sound )
	: Actor( TYPE_SOUND, name )
	, m_sound( sound ) {
	Rewind();
}

Sound::~Sound() {

}

const types::Sound* Sound::GetSound() const {
	return m_sound;
}

void Sound::Rewind() {
	m_pos = 0;
	m_is_finished = false;
	if ( m_is_autoplay ) {
		Play();
	}
}

void Sound::GetNextBuffer( uint8_t* buffer, int len ) {

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
	if ( m_is_muted ) {
		memset( ptr( buffer, 0, len ), 0, len );
	}
	else {
		memcpy( ptr( buffer, 0, len ), ptr( m_sound->m_buffer, m_pos, len ), len );
	}
	m_pos += len;
	if ( m_is_finished && m_is_repeatable ) {
		Rewind();

		// need to test this first (maybe not needed at all)
		//GetNextBuffer( ptr( buffer, newlen, len - newlen ), newlen );
	}
}

void Sound::SetRepeatable( const bool repeatable ) {
	m_is_repeatable = repeatable;
	Rewind();
}

void Sound::SetStartDelay( const size_t start_delay ) {
	m_start_delay = start_delay;
	Rewind();
}

void Sound::SetAutoPlay( const bool autoplay ) {
	m_is_autoplay = autoplay;
	Rewind();
}

void Sound::SetVolume( const float volume ) {
	ASSERT( volume >= 0 && volume <= 1, "invalid volume " + std::to_string( volume ) );
	m_volume = volume;
}

void Sound::Play() {
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

void Sound::Pause() {
	if ( m_is_playing ) {
		m_is_playing = false;
		m_is_active = false;
	}
}

void Sound::Stop() {
	m_start_delay_timer.Stop();
	m_is_playing = false;
	m_is_active = false;
	if ( m_is_repeatable ) {
		Rewind();
	}
	else {
		m_is_finished = true;
	}
}

void Sound::Mute() {
	m_is_muted = true;
}

const bool Sound::IsActive() {
	if ( !m_is_active ) {
		if ( m_start_delay_timer.HasTicked() ) {
			m_is_active = true;
		}
	}
	return m_is_active;
}

const bool Sound::IsFinished() const {
	return m_is_finished;
}

const bool Sound::IsReadyToBeDeleted() const {
	return ( m_is_repeatable || !m_is_playing );
}

const size_t Sound::GetPos() const {
	return m_pos;
}

const float Sound::GetVolume() const {
	return m_volume;
}

}
}
