#pragma once

namespace game {
namespace backend {

// TODO: get rid of this
enum tile_query_purpose_t {
	TQP_NONE,
	TQP_TILE_SELECT,
	TQP_UNIT_SELECT,
	TQP_OBJECT_SELECT, // unit or base
};

}
}
