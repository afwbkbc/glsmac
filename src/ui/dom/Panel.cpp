#include "Panel.h"

#include "Surface.h"

namespace ui {
namespace dom {

Panel::Panel( DOM_ARGS )
	: Container( DOM_ARGS_PASS, "panel" ) {

	m_surface = new Surface( ctx, call_si, ui, this, {} );
	Embed( m_surface );

	ForwardProperty( ctx, call_si, "background", m_surface );
}

Panel::~Panel() {

}

}
}
