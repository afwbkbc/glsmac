#include "Root.h"

#include "ui/geometry/Rectangle.h"

namespace ui {
namespace dom {

Root::Root( GSE_CALLABLE, UI* const ui )
	: Container( ctx, call_si, ui, nullptr, {}, "root" ) {} // the only dom object without parent

void Root::Resize( const uint16_t window_width, const uint16_t window_height ) {
	m_geometry->SetWidth( window_width );
	m_geometry->SetHeight( window_height );
}

}
}
