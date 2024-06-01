#pragma once

#include <unordered_map>

#include "SoundLoader.h"

namespace loader {
namespace sound {

CLASS( SDL2, SoundLoader )

	virtual ~SDL2();

protected:
	types::Sound* LoadSoundImpl( const std::string& filename ) override;

private:
	// cache all sounds for future use
	typedef std::unordered_map< std::string, types::Sound* > sound_map_t;
	sound_map_t m_sounds;
};

}
}
