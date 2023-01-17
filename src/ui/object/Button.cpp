#include "Button.h"

namespace ui {
namespace object {

Button::Button( const string& class_name ) : Panel( class_name ) {
	m_is_focusable = true;
}

}
}
