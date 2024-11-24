#pragma once

#include <vector>
#include <string>

#include "common/Task.h"

namespace ui {
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
	::ui::theme::Theme* m_theme;
	Style* m_style;

};

}
}
