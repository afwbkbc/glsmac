#include "Theme.h"

namespace game {
namespace frontend {
namespace ui {
namespace style {

Theme::Theme()
	: ::ui::theme::Theme() {

	AddStyleSheet( &m_styles.style );
	AddStyleSheet( &m_styles.bottom_bar );
	AddStyleSheet( &m_styles.popup );
}

}
}
}
}
