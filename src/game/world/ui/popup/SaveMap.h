#pragma once

#include "OkCancelPopup.h"

#include "ui/object/FileBrowser.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

CLASS( SaveMap, OkCancelPopup )
	
	SaveMap( World* world );
	
	void Create();
	void Destroy();

protected:
	void OnOk();
	
private:
	::ui::object::FileBrowser* m_file_browser = nullptr;
	
	void SelectFile( const std::string& file );
};

}
}
}
}
