#include "Capture.h"

#include "scene/Camera.h"
#include "types/texture/Texture.h"

namespace rr {

Capture::~Capture() {
	if ( camera ) {
		DELETE( camera );
	}
	if ( texture ) {
		DELETE( texture );
	}
}

}