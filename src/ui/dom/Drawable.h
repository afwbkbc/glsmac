#pragma once

#include <vector>

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

	geometry::Geometry* const GetGeometry() const override;

	void GeometryHandler( const geometry_handler_type_t type, const std::function< void() >& f );

	virtual const bool ProcessEvent( GSE_CALLABLE, const input::Event& event ) override;

	void Show() override;
	void Hide() override;

protected:

	virtual ~Drawable();

	geometry::Geometry* const m_geometry = nullptr;
	bool m_is_mouse_over = false;

private:

	std::vector< geometry_handler_id_t > m_geometry_handler_ids = {};

};

}
}
