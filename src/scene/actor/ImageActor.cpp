#include "ImageActor.h"

namespace scene {
namespace actor {

ImageActor::ImageActor( types::Texture *image )
	: Actor( Actor::TYPE_IMAGE, "Image" )
	, m_image( image )
{
	
}

ImageActor::~ImageActor() {
	
}

}
}
