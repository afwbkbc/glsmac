#pragma once

#include <string>

namespace game {
namespace backend {
namespace faction {

struct bases_render_info_t {
	std::string file = "";
	size_t grid_x = 0;
	size_t grid_y = 0;
	size_t cell_width = 0;
	size_t cell_height = 0;
	size_t cell_cx = 0;
	size_t cell_cy = 0;
	size_t cell_padding = 0;
	float scale_x = 1.0f;
	float scale_y = 1.0f;
};

}
}
}
