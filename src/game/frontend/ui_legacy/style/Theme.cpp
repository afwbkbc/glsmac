#include "Theme.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace style {

Theme::Theme()
	: ::ui_legacy::theme::Theme() {

	AddStyleSheet( &m_styles.style );
	AddStyleSheet( &m_styles.bottom_bar );
	AddStyleSheet( &m_styles.popup );
	AddStyleSheet( &m_styles.base_popup );
}

}
}
}
}
