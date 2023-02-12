#pragma once

#include "types/mesh/Rectangle.h"

#include "scene/actor/Mesh.h"

#include "UIObject.h"

#include "types/Color.h"
#include "types/Texture.h"

namespace ui {
namespace object {

CLASS( Mesh, UIObject )

	Mesh( const std::string& class_name = "" );
	
	void SetMesh( const types::mesh::Mesh* mesh );
	void SetTexture( const types::Texture* texture);
	void ClearTexture();
	
	virtual void Create();
	virtual void Destroy();
	virtual void Draw();
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
	
	const types::Texture* m_texture = nullptr;

	scene::actor::Mesh *m_actor = nullptr;
};

} /* namespace object */
} /* namespace ui */
