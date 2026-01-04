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

	Surface( DOM_ARGS_T = "surface" );

protected:
	virtual ~Surface();

	struct {
		types::texture::Texture* texture = nullptr;
		bool is_texture_owned = false;
	} m_background = {};

	void CreateTexture();
	
private:
	types::mesh::Rectangle* m_mesh = nullptr;
	scene::actor::Mesh* m_actor = nullptr;

	void ClearTexture();

private:
	friend class Panel;
	types::texture::Texture* GetOwnedTexturePtr();

};

}
}
