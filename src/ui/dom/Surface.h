#pragma once

#include "Object.h"

namespace types::mesh {
class Rectangle;
}

namespace scene::actor {
class Mesh;
}

namespace ui {

class Geometry;

namespace dom {

class Surface : public Object {
public:
	Surface( GSE_CALLABLE, UI* const ui, const std::string& tag, Object* const parent, const properties_t& properties );
	virtual ~Surface();

private:
	types::mesh::Rectangle* m_mesh = nullptr;
	scene::actor::Mesh* m_actor = nullptr;
	Geometry* m_geometry = nullptr;
};

}
}
