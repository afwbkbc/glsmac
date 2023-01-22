#pragma once

#include "SoundLoader.h"

#include <unordered_map>

using namespace std;

namespace loader {
namespace sound {

CLASS( SDL2, SoundLoader )
	
	virtual ~SDL2();
	
	types::Sound* LoadSound( const std::string &name );

private:
	// cache all sounds for future use
	typedef unordered_map< string, types::Sound* > sound_map_t;
	sound_map_t m_sounds;
};

}
}
