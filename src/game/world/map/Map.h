#pragma once

#include "base/Base.h"

#include <vector>
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
	vector<actor::Mesh*> GetActors() const;
	
private:
	
	// coordinates of textures (x1 and y1) in texture.pcx
	typedef Vec2< size_t > tc_t;
	const struct {
		const tc_t water[2] = {
			{ 280, 79 }, {280, 136 },
		};
		const tc_t rocks[4] = {
			{ 1, 1 }, { 58, 1 }, { 115, 1 }, { 172, 1 }
		};
		const tc_t dunes[1] = {
			{ 229, 1 }
		};
		const tc_t arid[1] = {
			{ 1, 58 }
		};
		const tc_t moist[16] = {
			{ 1, 115 }, { 58, 115 }, { 115, 115 }, { 172, 115 },
			{ 1, 172 }, { 58, 172 }, { 115, 172 }, { 172, 172 },
			{ 1, 229 }, { 58, 229 }, { 115, 229 }, { 172, 229 },
			{ 1, 286 }, { 58, 286 }, { 115, 286 }, { 172, 286 },
		};
		const tc_t rainy[16] = {
			{ 1, 343 }, { 58, 343 }, { 115, 343 }, { 172, 343 },
			{ 1, 400 }, { 58, 400 }, { 115, 400 }, { 172, 400 },
			{ 1, 457 }, { 58, 457 }, { 115, 457 }, { 172, 457 },
			{ 1, 514 }, { 58, 514 }, { 115, 514 }, { 172, 514 },
		};
		const tc_t jungle[16] = {
			{ 526, 259 }, { 583, 259 }, { 640, 259 }, { 697, 259 },
			{ 526, 316 }, { 583, 316 }, { 640, 316 }, { 697, 316 },
			{ 526, 373 }, { 583, 373 }, { 640, 373 }, { 697, 373 },
			{ 526, 430 }, { 583, 430 }, { 640, 430 }, { 959, 463 },
		};
		const tc_t fungus_land[16] = {
			{ 280, 515 }, { 337, 515 }, { 394, 515 }, { 451, 515 },
			{ 280, 572 }, { 337, 572 }, { 394, 572 }, { 451, 572 },
			{ 280, 629 }, { 337, 629 }, { 394, 629 }, { 451, 629 },
			{ 280, 686 }, { 337, 686 }, { 394, 686 }, { 451, 686 },
		};
		const tc_t fungus_sea[16] = {
			{ 508, 515 }, { 565, 515 }, { 622, 515 }, { 679, 515 },
			{ 508, 572 }, { 565, 572 }, { 622, 572 }, { 679, 572 },
			{ 508, 629 }, { 565, 629 }, { 622, 629 }, { 679, 629 },
			{ 508, 686 }, { 565, 686 }, { 622, 686 }, { 679, 686 },
		};
	} m_tc;
	
	Tiles* m_tiles = nullptr;
	
	struct {
		Texture* source = nullptr;
		Texture* terrain = nullptr;
	} m_textures;
	
	Scene* m_scene = nullptr;
	struct {
		actor::Mesh* terrain = nullptr;
	} m_actors;
	
	void GenerateActors();
	void GenerateTerrainActor();
	void GenerateWaterActor();
	
	typedef struct {
		uint8_t inverse_x;
		uint8_t inverse_y;
		uint8_t rotate_direction;
		uint8_t texture_variant;
	} tile_texture_info_t;
	
	enum tile_grouping_criteria_t {
		TG_MOISTURE,
		TG_FEATURE,
	};
	
	unordered_map< uint8_t, uint8_t > m_texture_variants = {}; // cache
	
	const tile_texture_info_t GetTileTextureInfo( Tile* tile, const tile_grouping_criteria_t criteria, const Tile::feature_t feature = Tile::F_NONE ) const;

};

}
}
}
