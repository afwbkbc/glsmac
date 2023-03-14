#include "Theme.h"

namespace game {
namespace style {

Theme::Theme() : ui::theme::Theme() {
	
	AddStyleSheet( &m_styles.common );
	AddStyleSheet( &m_styles.popup_menu );
	AddStyleSheet( &m_styles.scrollbars );
	
}

}
}
