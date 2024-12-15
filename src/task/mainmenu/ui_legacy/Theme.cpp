#include "Theme.h"

namespace task {
namespace mainmenu {

Theme::Theme()
	: ui_legacy::theme::Theme() {

	AddStyleSheet( &m_styles.mainmenu_common );
	AddStyleSheet( &m_styles.sliding_menu );

}

}
}
