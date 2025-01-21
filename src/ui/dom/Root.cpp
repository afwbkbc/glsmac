#include "Root.h"

#include "ui/geometry/Rectangle.h"

namespace ui {
namespace dom {

Root::Root( GSE_CALLABLE, UI* const ui )
	: Container( ctx, call_si, ui, nullptr, {}, "root", true ) { // the only dom object without parent

	Events(
		{
			input::EV_KEY_DOWN,
		}
	);

	InitAndValidate( ctx, call_si );
}

void Root::Resize( const uint16_t window_width, const uint16_t window_height ) {
	m_geometry->SetWidth( window_width );
	m_geometry->SetHeight( window_height );
}

}
}
