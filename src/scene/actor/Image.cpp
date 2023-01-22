#include "Image.h"

namespace scene {
namespace actor {

Image::Image( types::Texture *image )
	: Actor( Actor::TYPE_IMAGE, "Image" )
	, m_image( image )
{
	
}

Image::~Image() {
	
}

}
}
