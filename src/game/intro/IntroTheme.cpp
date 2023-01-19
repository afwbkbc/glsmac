#include "IntroTheme.h"

namespace game {
namespace intro {

IntroTheme::IntroTheme() : ui::theme::Theme() {
	AddStyleSheet( &m_style );
}

}
}
