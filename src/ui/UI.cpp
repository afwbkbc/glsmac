#include "UI.h"

#include "dom/Root.h"

namespace ui {

UI::UI() {
	m_root = new dom::Root();
}

UI::~UI() {
	delete m_root;
}

WRAPIMPL_BEGIN( UI )
	WRAPIMPL_PROPS
			{
				"root",
				m_root->Wrap( true )
			},
		};
WRAPIMPL_END_PTR( GLSMAC )

}
