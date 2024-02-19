#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "game/map/Map.h"
#include "game/map/TileState.h"
#include "types/Vec3.h"
#include "types/mesh/Render.h"

namespace task {
namespace game {

struct unit_data_t {
	types::mesh::Mesh* preview_mesh = nullptr;
	types::Texture* preview_texture = nullptr;
};

struct tile_data_t {
	bool is_set = false;
	Vec2< size_t > tile_position = {};
	Vec3 coords = {};
	::game::map::TileState::tile_vertices_t selection_coords = {};
	std::vector< types::mesh::Render* > preview_meshes = {};
	std::vector< std::string > preview_lines = {};
	std::vector< std::string > sprites = {};
	bool scroll_adaptively = false;
	std::vector< unit_data_t > units = {};
};

}
}
