#include "Button.h"

#include "Text.h"
#include "ui/geometry/Geometry.h"

namespace ui {
namespace dom {

Button::Button( DOM_ARGS )
	: Panel( DOM_ARGS_PASS, "button", false ) {

	m_label = new Text( ctx, call_si, ui, this, {} );
	m_label->GetGeometry()->SetAlign( geometry::Geometry::ALIGN_CENTER );
	Embed( m_label );

	ForwardProperty( ctx, call_si, "text", m_label );
	ForwardProperty( ctx, call_si, "color", m_label );
	ForwardProperty( ctx, call_si, "font", m_label );

}

}
}
