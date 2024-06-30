#pragma once

#include <cstdint>

#include "types/Vec2.h"

namespace types {
namespace mesh {

typedef float coord_t;
typedef uint32_t index_t;
typedef uint32_t surface_id_t;

typedef coord_t tex_coord_t;
typedef std::pair< Vec2< tex_coord_t >, Vec2< tex_coord_t > > tex_coords_t;

typedef uint32_t data_t;

}
}
