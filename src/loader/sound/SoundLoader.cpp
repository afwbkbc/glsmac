#include "SoundLoader.h"

namespace loader {
namespace sound {

types::Sound* SoundLoader::LoadSound( const resource::resource_t res ) {
	return LoadSoundImpl( GetFilename( res ) );
}

types::Sound* SoundLoader::LoadCustomSound( const std::string& filename ) {
	return LoadSoundImpl( GetCustomFilename( filename ) );
}

}
}