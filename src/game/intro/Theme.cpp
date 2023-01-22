#include "Theme.h"

namespace game {
namespace intro {

Theme::Theme() : ui::theme::Theme() {
	AddStyleSheet( &m_style );
}

}
}
