#include "Theme.h"

namespace task {
namespace mainmenu {

using namespace style;

Theme::Theme()
	: ui::theme::Theme() {

	AddStyleSheet( &m_styles.mainmenu_common );
	AddStyleSheet( &m_styles.sliding_menu );

}

}
}
