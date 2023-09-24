#pragma once

#include <string>

#include "../PopupMenu.h"

#include "base/MTModule.h"

//#include "ui/object/Section.h"
//#include "ui/object/ChoiceList.h"

#include "ui/object/FileBrowser.h"

namespace task {

using namespace ui;

namespace mainmenu {

CLASS( LoadMapFile, PopupMenu )

	LoadMapFile( MainMenu* mainmenu );
	~LoadMapFile();

	void Show() override;
	void Align() override;
	void Hide() override;

	void OnNext() override;

private:
	Section* m_section = nullptr;
	::ui::object::FileBrowser* m_file_browser = nullptr;

	mt_id_t m_mt_id = 0;
};

}
}
