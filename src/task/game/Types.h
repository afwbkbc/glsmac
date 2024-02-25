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

struct meshtex_t {
	const types::mesh::Mesh* mesh = nullptr;
	types::Texture* texture = nullptr;
};

struct unit_data_t {
	size_t id;
	meshtex_t unit;
	meshtex_t badge;
	meshtex_t healthbar;
	std::string unit_name;
	std::string short_power_string;
	std::string morale_string;
	std::string moves_string;
};

struct tile_data_t {
	bool is_set = false;
	::game::tile_query_purpose_t purpose = ::game::TQP_NONE;
	Vec2< size_t > tile_position = {};
	Vec3 coords = {};
	::game::map::TileState::tile_vertices_t selection_coords = {};
	std::vector< types::mesh::Render* > preview_meshes = {};
	std::vector< std::string > preview_lines = {};
	std::vector< std::string > sprites = {};
	bool scroll_adaptively = false;
	std::vector< unit_data_t > units = {};
	::game::tile_query_metadata_t metadata = {};
};

}
}
