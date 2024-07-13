#pragma once

#include "Module.h"

#include "types/texture/Types.h"

namespace game {
namespace backend {
namespace map {
namespace module {

class Coastlines : public Module {
public:
	Coastlines( Map* const map )
		: Module( map ) {}

protected:
	struct coastline_corner_t {
		size_t msx = 0;
		size_t msy = 0;
		types::texture::add_flag_t flags = 0;
		bool can_mirror = false;
		bool maybe_mirror_nw = false;
		bool maybe_mirror_ne = false;
		bool maybe_mirror_se = false;
		bool maybe_mirror_sw = false;
		types::texture::add_flag_t mirror_mode;
	};
};

}
}
}
}
