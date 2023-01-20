#pragma once

#include <string>

#include "../Loader.h"

#include "types/Sound.h"

namespace loader {
namespace sound {

MAJOR_CLASS( SoundLoader, Loader )
	virtual types::Sound* LoadSound( const std::string &name ) = 0;
};

} /* namespace sound */
} /* namespace loader */
