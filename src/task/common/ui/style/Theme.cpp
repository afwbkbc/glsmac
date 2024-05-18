#include "Theme.h"

#include "Common.h"
#include "Popup.h"
#include "Scrollbars.h"

namespace task {
namespace style {

Theme::Theme()
	: ui::theme::Theme() {

	NEW( m_styles.common, Common );
	AddStyleSheet( m_styles.common );
	NEW( m_styles.popup, Popup );
	AddStyleSheet( m_styles.popup );
	NEW( m_styles.scrollbars, Scrollbars );
	AddStyleSheet( m_styles.scrollbars );

}

Theme::~Theme() {
	DELETE( m_styles.common );
	DELETE( m_styles.popup );
	DELETE( m_styles.scrollbars );
}

}
}
