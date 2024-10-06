#pragma once

#include <string>
#include <cstdint>

namespace game {
namespace backend {
namespace animation {

enum animation_type_t {
	AT_NONE,
	AT_FRAMES_ROW,
};

struct sprite_render_info_t {
	std::string file;
	uint16_t row_x;
	uint16_t row_y;
	uint16_t frame_width;
	uint16_t frame_height;
	uint16_t frame_padding;
	uint16_t frame_center_x;
	uint16_t frame_center_y;
	uint8_t frames_count;
	uint8_t frames_per_row;
	float scale_x;
	float scale_y;
};

}
}
}
