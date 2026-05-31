#pragma once

#include <vector>
#include <cstdint>
#include <string>

#include "types/Vec2.h"

namespace game {
namespace backend {
namespace resource {

typedef std::vector< std::pair< types::Vec2< int64_t >, types::Vec2< int64_t > > > render_coords_t;

struct render_info_t {
	std::string file;
	render_coords_t coords;
};

}
}
}
