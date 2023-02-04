#include "Theme.h"

namespace game {
namespace world {
namespace ui {

Theme::Theme() : ::ui::theme::Theme() {
	
	AddStyleSheet( &m_styles.style );

}

}
}
}
