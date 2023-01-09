#pragma once

#include "Actor.h"

#include "types/Texture.h"

namespace scene {
namespace actor {

class ImageActor : public Actor {
public:
	ImageActor( types::Texture *image );
	~ImageActor();
	types::Texture *GetImage() const {
		return m_image;
	}
protected:
	types::Texture *m_image;
};

} /* namespace actor */
} /* namespace scene */
