#pragma once

#include <vector>
#include <string>

#include "common/Task.h"

namespace ui_legacy {
namespace theme {
class Theme;
}
}

namespace task {
namespace console {

class UI;
class Style;

CLASS( Console, common::Task )

	void Start() override;
	void Stop() override;

	static void Log( const std::string& text );

private:
	UI* m_ui;
	::ui_legacy::theme::Theme* m_theme;
	Style* m_style;

};

}
}
