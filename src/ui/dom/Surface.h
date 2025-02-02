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

namespace dom {

class Surface : public Area {
public:
	Surface( DOM_ARGS );

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

private:
	friend class Panel;
	types::texture::Texture* GetOwnedTexturePtr();

};

}
}
