#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace types {
class Buffer;
}

namespace game {
namespace backend {
namespace base {

class RenderInfo {
public:
	std::string file;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	const std::string ToString( const std::string& prefix ) const;
	void Write( types::Buffer& buf ) const;
};
typedef std::vector< RenderInfo > render_infos_t;

}
}
}
