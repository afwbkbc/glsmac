#pragma once

#include "base/Base.h"

#include <unordered_map>

#include "Tiles.h"

#include "types/Texture.h"
#include "scene/actor/Mesh.h"
#include "scene/Scene.h"

using namespace types;
using namespace scene;

namespace game {
namespace world {
namespace map {
	
CLASS( Map, base::Base )

	Map( Scene* constscene );
	~Map();
	
	void SetTiles( Tiles* tiles );
	
	// tmp
	actor::Mesh* GetActor() const;
	
private:
	Scene* m_scene = nullptr;
	
	Tiles* m_tiles = nullptr;

	vector< Texture* > m_textures;
	
	struct {
		unordered_map<Tile::moisture_t, Texture*> moisture;
		unordered_map<Tile::rockyness_t, Texture*> rockyness;
	} m_texture_parts;
	
	actor::Mesh* m_terrain_actor = nullptr;

	void GenerateActor();

};

}
}
}
