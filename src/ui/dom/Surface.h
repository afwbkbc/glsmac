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
	virtual ~Surface();

private:
	types::mesh::Rectangle* m_mesh = nullptr;
	scene::actor::Mesh* m_actor = nullptr;

	struct {
		types::texture::Texture* color_texture = nullptr; // this one is owned
		types::texture::Texture* texture = nullptr; // this one isn't
	} m_background = {};

};

}
}
