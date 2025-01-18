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
	Surface* m_border_surface;

	long int m_border_corners = 0;
	types::Color m_border_color = {};

	void UpdateBorderTexture();
};

}
}
