#pragma once

#include "Surface.h"

#include "types/Texture.h"

namespace ui {
namespace object {

CHILD_CLASS( Image, Surface )
	Image( const std::string& filename );

protected:
	const types::Texture* m_texture;
};

} /* namespace object */
} /* namespace ui */
