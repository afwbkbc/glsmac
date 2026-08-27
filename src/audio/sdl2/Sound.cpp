#include "Sound.h"

#include "scene/actor/Sound.h"

namespace audio {
namespace sdl2 {

Sound::Sound( scene::actor::Sound* actor )
	: m_actor( actor ) {

}

Sound::~Sound() {

}

const bool Sound::IsActive() {
	return m_actor->IsActive();
}

const float Sound::GetVolume() const {
	return m_actor->GetVolume();
}

void Sound::GetNextBuffer( uint8_t* buffer, size_t len ) {
	m_actor->GetNextBuffer( buffer, len );
}

}
}
