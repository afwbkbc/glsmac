#include "Theme.h"

namespace task {
namespace intro {

Theme::Theme()
	: ui_legacy::theme::Theme() {
	AddStyleSheet( &m_style );
}

}
}
