#include "Theme.h"

namespace task {
namespace intro {

Theme::Theme()
	: ui::theme::Theme() {
	AddStyleSheet( &m_style );
}

}
}
