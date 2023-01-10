#pragma once

/* Clickable button */

#include "Panel.h"

#include "Surface.h"

namespace ui {
namespace object {

CHILD_CLASS( Button, Panel )
	/*virtual void Create() {
		UIContainer::Create();

		AddChild( m_background = new Surface );
	}
	virtual void Destroy() {
		RemoveChild( m_background );

		UIContainer::Destroy();
	}*/
protected:
	//Surface *m_background;
};

} /* namespace object */
} /* namespace ui */
