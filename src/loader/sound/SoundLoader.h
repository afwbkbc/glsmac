#pragma once

#include <string>

#include "loader/Loader.h"

namespace types {
class Sound;
}

namespace loader {
namespace sound {

CLASS( SoundLoader, Loader )
	virtual types::Sound* LoadSound( const std::string& name ) = 0;
};

}
}
