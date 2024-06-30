#include "FactionTXTLoader.h"

#include "engine/Engine.h"
#include "resource/ResourceManager.h"

namespace loader {
namespace txt {

const FactionTXTLoader::faction_data_t& FactionTXTLoader::GetFactionData( resource::resource_t res ) {
	return GetFactionDataImpl( g_engine->GetResourceManager()->GetPath( res ) );
}

const FactionTXTLoader::faction_data_t& FactionTXTLoader::GetFactionData( const std::string& filename ) {
	return GetFactionDataImpl( g_engine->GetResourceManager()->GetCustomPath( filename ) );
}

const FactionTXTLoader::faction_data_t& FactionTXTLoader::GetFactionDataImpl( const std::string& path ) {
	auto it = m_faction_data.find( path );
	if ( it == m_faction_data.end() ) {
		const auto data = GetTXTData( path );
		it = m_faction_data.insert(
			{
				path,
				faction_data_t{
					{
						GetSection( data, "BASES" ),
						GetSection( data, "WATERBASES" ),
					}
				}
			}
		).first;
	}
	return it->second;
}

const std::vector< std::string >& FactionTXTLoader::GetSection( const txt_data_t& data, const std::string& name ) const {
	const auto bases_it = data.sections.find( name );
	if ( bases_it == data.sections.end() ) {
		THROW( "file does not contain section #" + name );
	}
	return bases_it->second;
}

}
}
