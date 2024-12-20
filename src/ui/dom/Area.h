#pragma once

#include "Object.h"

namespace ui {
namespace dom {

class Root;

class Area : public Object {
public:
	Area( DOM_ARGS_T );
	virtual ~Area();

	Geometry* const GetGeometry() const override;

protected:
	Geometry* const m_geometry = nullptr;

};

}
}
