#pragma once

#include <string>

#include "SoundLoader.h"

#include "types/Sound.h"

namespace loader {
namespace sound {

CLASS( Null, SoundLoader )
protected:
	types::Sound* LoadSoundImpl( const std::string& name ) override { return nullptr; }
};

}
}
