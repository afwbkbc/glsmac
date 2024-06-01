#pragma once

#include <string>

#include "loader/Loader.h"

namespace types {
class Sound;
}

namespace loader {
namespace sound {

CLASS( SoundLoader, Loader )

	types::Sound* LoadSound( const resource::resource_t res );
	types::Sound* LoadCustomSound( const std::string& filename );

protected:
	virtual types::Sound* LoadSoundImpl( const std::string& filename ) = 0;
};

}
}
