#pragma once

#include "types/mesh/Rectangle.h"
#include "types/Vec2.h"

#include "Mesh.h"

namespace ui {
namespace object {

CLASS( Surface, Mesh )

	Surface( const std::string& class_name = "" );
	
	void SetTexture( types::Texture* texture ) override;
	void SetStretchTexture( const bool stretch_texture );
	
	virtual void Create() override;
	virtual void Iterate() override;
	virtual void Align() override;
	
protected:
	
	void Resize();
	void ResizeNow();
	virtual void ApplyStyle() override;
	
	bool m_need_resize = false;
	
	bool m_stretch_texture = false;

	types::mesh::Rectangle *m_background_mesh = nullptr;
	
	object_area_t m_surface_object_area = {};
	Vec2< size_t > m_surface_texture_size = { 0, 0 };
	bool m_surface_stretch_texture = false;
};

} /* namespace object */
} /* namespace ui */
