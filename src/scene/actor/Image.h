#pragma once

#include "Actor.h"

#include "types/Texture.h"

namespace scene {
namespace actor {

CLASS( Image, Actor )
	Image( types::Texture *image );
	~Image();
	types::Texture *GetImage() const {
		return m_image;
	}
protected:
	types::Texture *m_image;
};

} /* namespace actor */
} /* namespace scene */
