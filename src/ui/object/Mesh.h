#pragma once

#include "types/mesh/Rectangle.h"
#include "types/Color.h"

#include "scene/actor/Mesh.h"

#include "UIObject.h"

#include "types/Color.h"
#include "types/Texture.h"

namespace ui {
namespace object {

CLASS( Mesh, UIObject )

	Mesh( const std::string& class_name = "" );
	
	void SetMesh( const types::mesh::Mesh* mesh );
	virtual void SetTexture( types::Texture* texture );
	void ClearTexture();
	
	void SetTintColor( const types::Color color );
	
	virtual void Destroy();
	virtual void Align();
	
	enum aspect_ratio_mode_t {
		AM_NONE,
		AM_SCALE_WIDTH, // width can be decreased to keep aspect ratio
		AM_SCALE_HEIGHT // height can be decreased to keep aspect ratio
	};
	void SetAspectRatioMode( const aspect_ratio_mode_t mode );
	
protected:
	
	// one if kept as-is, other gets resized on align
	const types::mesh::Mesh* m_original_mesh = nullptr;
	types::mesh::Mesh* m_mesh = nullptr;
	
	aspect_ratio_mode_t m_aspect_ratio_mode = AM_NONE;
	
	types::Texture* m_texture = nullptr;

	scene::actor::Mesh *m_actor = nullptr;
	
	struct {
		bool enabled = false;
		types::Color color;
	} m_tint_color = {};
	
	bool m_force_resize = false;
	object_area_t m_last_object_area = { 0, 0 };
	Vec2< size_t > m_last_viewport_size = { 0, 0 };
	coord_t m_last_margin = 0;
	types::mesh::Mesh* m_last_mesh = nullptr;
	Vec2< size_t > m_last_texture_size = { 0, 0 };
	size_t m_last_mesh_updated_count = 0;
	
	void UpdateRenderFlags();
};

} /* namespace object */
} /* namespace ui */
