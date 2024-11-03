#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace game {
namespace backend {
namespace base {

struct pop_render_info_t {
	std::string file;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
};
typedef std::vector< pop_render_info_t > pop_render_infos_t;

}
}
}
