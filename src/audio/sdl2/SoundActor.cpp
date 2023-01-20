#include "SoundActor.h"
#include "engine/Engine.h"

#include "SDL2Audio.h"

namespace audio {
namespace sdl2 {


SoundActor::SoundActor( scene::actor::SoundActor* actor )
	: m_actor( actor )
{
	
}

SoundActor::~SoundActor() {
	
}

const bool SoundActor::IsActive() {
	return m_actor->IsActive();
}

const float SoundActor::GetVolume() const {
	return m_actor->GetVolume();
}

void SoundActor::GetNextBuffer( uint8_t* buffer, int len ) {
	m_actor->GetNextBuffer( buffer, len );
}

}
}
