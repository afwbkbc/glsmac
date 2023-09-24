#pragma once

#include <unordered_map>

#include "SoundLoader.h"

namespace loader {
namespace sound {

CLASS( SDL2, SoundLoader )

	virtual ~SDL2();

	types::Sound* LoadSound( const std::string& name ) override;

private:
	// cache all sounds for future use
	typedef std::unordered_map< std::string, types::Sound* > sound_map_t;
	sound_map_t m_sounds;
};

}
}
