#pragma once

#include "Area.h"

namespace types::mesh {
class Rectangle;
}

namespace scene::actor {
class Mesh;
}

namespace ui {

class Geometry;

namespace dom {

class Surface : public Area {
public:
	Surface( DOM_ARGS_T );
	virtual ~Surface();

private:
	types::mesh::Rectangle* m_mesh = nullptr;
	scene::actor::Mesh* m_actor = nullptr;
};

}
}
