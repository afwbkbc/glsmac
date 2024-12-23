#pragma once

#include "Geometry.h"

namespace types::mesh {
class Rectangle;
}

namespace ui {
namespace geometry {

class Rectangle : public Geometry {
public:
	Rectangle( const UI* const ui, Geometry* const parent );

	void SetMesh( types::mesh::Rectangle* const mesh );

protected:
	void UpdateImpl() override;

private:

	types::mesh::Rectangle* m_mesh = nullptr;

};

}
}
