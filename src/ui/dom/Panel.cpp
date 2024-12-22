#include "Panel.h"

#include "Surface.h"

namespace ui {
namespace dom {

Panel::Panel( DOM_ARGS )
	: Container( DOM_ARGS_PASS, "panel" ) {

	m_surface = new Surface( ctx, call_si, ui, this, {} );

	ForwardProperty( ctx, call_si, "background", m_surface );
}

Panel::~Panel() {
	delete m_surface;
}

}
}