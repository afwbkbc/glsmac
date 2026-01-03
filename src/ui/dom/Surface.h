#pragma once

#include "Area.h"

namespace types {
namespace mesh {
class Rectangle;
}
namespace texture {
class Texture;
}
}

namespace scene::actor {
class Mesh;
}

namespace ui {

class Geometry;
class UI;

namespace dom {

class Surface : public Area {
public:

	static const std::string UI_CLASS;

	Surface( DOM_ARGS );

	void Destroy( GSE_CALLABLE ) override;

protected:
	virtual ~Surface();

private:
	types::mesh::Rectangle* m_mesh = nullptr;
	scene::actor::Mesh* m_actor = nullptr;

	struct {
		types::texture::Texture* texture = nullptr;
		bool is_texture_owned = false;
	} m_background = {};

	void CreateTexture();
	void ClearTexture();

	bool m_is_render_surface = false;

private:
	friend class Panel;
	types::texture::Texture* GetOwnedTexturePtr();

private:
	friend class ui::UI;
	void EnableRenderSurface();
	void DisableRenderSurface();
	types::texture::Texture* const GetTextureForRender() const;

};

}
}
