#pragma once

#include "Module.h"

namespace game {
namespace map {
namespace module {

class Coastlines : public Module {
public:
	Coastlines( Map* const map ) : Module( map ) {}
	
protected:
	struct coastline_corner_t {
		size_t msx = 0;
		size_t msy = 0;
		Texture::add_flag_t flags = 0;
		bool can_mirror = false;
		bool maybe_mirror_nw = false;
		bool maybe_mirror_ne = false;
		bool maybe_mirror_se = false;
		bool maybe_mirror_sw = false;
		Texture::add_flag_t mirror_mode;
	};
};

}
}
}
