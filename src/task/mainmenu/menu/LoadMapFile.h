#pragma once

#include "task/mainmenu/PopupMenu.h"

namespace ui_legacy::object {
class Section;
class FileBrowser;
}

namespace task {
namespace mainmenu {

CLASS( LoadMapFile, PopupMenu )

	LoadMapFile( MainMenu* mainmenu );
	~LoadMapFile();

	void Show() override;
	void Align() override;
	void Hide() override;

	void OnNext() override;

private:

	ui_legacy::object::Section* m_section = nullptr;
	::ui_legacy::object::FileBrowser* m_file_browser = nullptr;

};

}
}
