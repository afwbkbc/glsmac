#pragma once

#include "Object.h"

namespace ui {

namespace geometry {
class Geometry;
}

namespace dom {

class Root;

class Drawable : public Object {
public:
	Drawable( DOM_ARGS_T, geometry::Geometry* const geometry );
	virtual ~Drawable();

	geometry::Geometry* const GetGeometry() const override;

protected:
	geometry::Geometry* const m_geometry = nullptr;

};

}
}
