#pragma once

#include "types/mesh/Rectangle.h"

#include "Mesh.h"

namespace ui {
namespace object {

CLASS( Surface, Mesh )

	Surface( const std::string& class_name = "" );
	
	void SetStretchTexture( const bool stretch_texture );
	
	virtual void Create();
	virtual void Align();
	
	
protected:
	
	virtual void ApplyStyle();
	
	bool m_stretch_texture = false;

	types::mesh::Rectangle *m_background_mesh = nullptr;
};

} /* namespace object */
} /* namespace ui */
