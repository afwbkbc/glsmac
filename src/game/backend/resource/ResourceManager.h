#pragma once

#include <vector>
#include <unordered_map>

#include "common/Common.h"
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

CLASS2( ResourceManager, common::Class, gse::GCWrappable )

	ResourceManager( Game* game );
	~ResourceManager();

	void Clear();
	void DefineResource( resource::Resource* resource );

	const map::tile::yields_t GetYields( GSE_CALLABLE, map::tile::Tile* tile, slot::Slot* slot );

	WRAPDEFS_PTR( ResourceManager )

	void Serialize( types::Buffer& buf ) const;
	void Unserialize( types::Buffer& buf );

private:
	Game* m_game;

	std::unordered_map< std::string, resource::Resource* > m_resources = {};
	std::vector< std::string > m_resource_idx = {};
	std::unordered_map< std::string, size_t > m_resource_idx_map = {};
};

}
}
}
