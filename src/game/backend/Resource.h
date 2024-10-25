#pragma once

#include <string>
#include <cstdint>

#include "types/Buffer.h"

namespace game {
namespace backend {

class Resource {
public:

	struct render_info_t {
		std::string file;
		struct {
			uint16_t grid_x;
			uint16_t grid_y;
			uint8_t grid_margin;
			uint16_t cell_width;
			uint16_t cell_height;
			uint8_t cells_count;
		} yields;
#define X( _n ) \
        struct { \
            uint16_t x; \
            uint16_t y; \
            uint16_t width; \
            uint16_t height; \
        } _n;
		X( plus )
		X( minus )
#undef X
	};

	Resource( const std::string& id, const std::string& name, const render_info_t& render_info );

	const std::string m_id;
	const std::string m_name;
	const render_info_t m_render_info;

	const std::string ToString( const std::string& prefix = "" ) const;

	static const types::Buffer Serialize( const Resource* resource );
	static Resource* Unserialize( types::Buffer& buf );

};

}
}
