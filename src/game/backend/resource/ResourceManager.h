#pragma once

#include <vector>
#include <unordered_map>

#include "gse/GCWrappable.h"
#include "gse/value/Object.h"
#include "game/backend/map/tile/Types.h"
#include "Types.h"

namespace game {
namespace backend {

class Game;

namespace slot {
class Slot;
}

namespace map::tile {
class Tile;
}

namespace resource {

class Resource;

CLASS( ResourceManager, gse::GCWrappable )

	typedef std::unordered_map< std::string, resource::Resource* > resource_definitions_t;

	ResourceManager( Game* game );
	~ResourceManager();

	void Clear();
	void DefineResource( resource::Resource* resource );
	void UndefineResource( const std::string& id );
	void DefineNoResource( const render_info_t& info );
	void UndefineNoResource();

	const resource_definitions_t& GetDefinedResources() const;
	const std::vector< std::string >& GetDefinedResourcesOrder() const;

	WRAPDEFS_PTR( ResourceManager )

	void Serialize( types::Buffer& buf ) const;
	void Deserialize( types::Buffer& buf );

private:
	Game* m_game;

	std::vector< std::string > m_resources_order = {};
	resource_definitions_t m_resources = {};

	bool m_noresource_defined = false;
	render_info_t m_noresource_info = {};

	void AddRenderCoords( GSE_CALLABLE, render_coords_t& render_coords, gse::Value* const value );
};

}
}
}
