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
	
	// coordinates of textures (x1 and y1) in texture.pcx
	typedef Vec2< size_t > tc_t;
	const struct {
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
	} m_tc;
	
	Scene* m_scene = nullptr;
	actor::Mesh* m_terrain_actor = nullptr;
	Texture* m_source_texture = nullptr;
	
	Tiles* m_tiles = nullptr;
	Texture* m_texture = nullptr;
	
	void GenerateActor();

};

}
}
}
