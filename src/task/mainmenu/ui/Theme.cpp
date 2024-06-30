#include "Theme.h"

namespace task {
namespace mainmenu {

Theme::Theme()
	: ui::theme::Theme() {

	AddStyleSheet( &m_styles.mainmenu_common );
	AddStyleSheet( &m_styles.sliding_menu );

}

}
}
