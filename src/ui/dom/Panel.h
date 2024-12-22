#pragma once

#include "Container.h"

namespace ui {
namespace dom {

class Surface;

class Panel : public Container {
public:
	Panel( DOM_ARGS );
	virtual ~Panel();

private:
	Surface* m_surface;

};

}
}
