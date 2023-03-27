#pragma once

#include "OkCancelPopup.h"

#include "ui/object/FileBrowser.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

CLASS( FilePopup, OkCancelPopup )
	
	enum file_mode_t {
		FM_NONE,
		FM_READ,
		FM_WRITE
	};
	
	FilePopup( World* world, const std::string& title, const file_mode_t file_mode, const std::string& default_directory = "" );
	
	void Create();
	void Destroy();

protected:
	void OnOk();
	
	virtual void OnFileSelect( const std::string& path ) {}
	
private:
	const std::string m_title = "";
	const file_mode_t m_file_mode = FM_NONE;
	const std::string m_default_directory = "";
	
	::ui::object::FileBrowser* m_file_browser = nullptr;

};

}
}
}
}
