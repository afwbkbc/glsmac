#include "Theme.h"

namespace game {
namespace world {
namespace ui {
namespace style {

Theme::Theme() : ::ui::theme::Theme() {
	
	AddStyleSheet( &m_styles.style );
	AddStyleSheet( &m_styles.bottom_bar );
	
}

}
}
}
}
