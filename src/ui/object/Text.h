#pragma once

namespace ui {
namespace object {

/* simple text label */

#include "scene/mesh/vec2/Rectangle.h"
#include "scene/actor/MeshActor.h"

#include "types/Color.h"

CHILD_CLASS( Surface, UIObject )
	void SetColor( const types::Color &color) {
		m_colors.text = color;
		Redraw();
	}
protected:
	struct {
		types::Color text;
	} m_colors;
};

} /* namespace object */
} /* namespace ui */
