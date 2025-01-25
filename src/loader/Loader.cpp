#include "Loader.h"

#include "engine/Engine.h"
#include "resource/ResourceManager.h"

namespace loader {

const std::string& Loader::GetFilename( const resource::resource_t res ) const {
	return g_engine->GetResourceManager()->GetFilename( res );
}

const std::string& Loader::GetPath( const resource::resource_t res ) const {
	return g_engine->GetResourceManager()->GetPath( res );
}

const std::string& Loader::TryGetCustomFilename( const std::string& filename ) const {
	return g_engine->GetResourceManager()->TryGetCustomPath( filename );
}

const std::string& Loader::GetCustomFilename( const std::string& filename ) const {
	return g_engine->GetResourceManager()->GetCustomPath( filename );
}

}
