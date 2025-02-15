#pragma once

#include "UIObject.h"

#include "types/Vec2.h"
#include "types/Color.h"

namespace types {
namespace texture {
class Texture;
}
namespace mesh {
class Mesh;
}
}

namespace scene::actor {
class Mesh;
}

namespace ui_legacy {
namespace object {

CLASS( Mesh, UIObject )

	Mesh( const std::string& class_name = "" );

	void SetMesh( const types::mesh::Mesh* mesh );
	virtual void SetTexture( types::texture::Texture* texture );
	void ClearTexture();

	const types::mesh::Mesh* GetOriginalMesh() const;

	void SetTintColor( const types::Color& color );
	void SetTintAlpha( const float alpha );

	virtual void Destroy() override;
	virtual void Align() override;

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

	types::texture::Texture* m_texture = nullptr;

	scene::actor::Mesh* m_actor = nullptr;

	struct {
		bool enabled = false;
		types::Color color;
	} m_tint_color = {};

	bool m_force_resize = false;
	object_area_t m_last_object_area = {
		0,
		0
	};
	types::Vec2< size_t > m_last_viewport_size = {
		0,
		0
	};
	coord_t m_last_margin = 0;
	types::mesh::Mesh* m_last_mesh = nullptr;
	types::Vec2< size_t > m_last_texture_size = {
		0,
		0
	};
	size_t m_last_mesh_updated_count = 0;

	void UpdateRenderFlags();
};

}
}
