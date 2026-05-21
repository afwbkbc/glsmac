#pragma once

#include <vector>
#include <unordered_map>

#include "gse/GCWrappable.h"

#include "gse/value/Object.h"

#include "game/backend/map/tile/Types.h"

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

	const resource_definitions_t& GetDefinedResources() const;

	WRAPDEFS_PTR( ResourceManager )

	void Serialize( types::Buffer& buf ) const;
	void Deserialize( types::Buffer& buf );

private:
	Game* m_game;

	resource_definitions_t m_resources = {};
};

}
}
}
