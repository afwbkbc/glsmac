#include "Image.h"

#include "engine/Engine.h"

namespace ui {
namespace object {


Image::Image(const std::string& filename ) 
	: Surface()
{
	m_texture = g_engine->GetTextureLoader()->LoadTexture( filename );
	SetTexture( m_texture );
	ForceAspectRatio( m_texture->m_aspect_ratio );
}

}
}
