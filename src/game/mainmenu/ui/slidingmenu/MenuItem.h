#pragma once

#include <vector>

#include "ui/object/UIContainer.h"
#include "ui/object/Surface.h"
#include "ui/object/Label.h"

using namespace ui::object;
using namespace ui::event;

namespace game {
namespace mainmenu {

class MenuBlock;
	
CLASS( MenuItem, UIContainer )
	MenuItem( MenuBlock* menu, const std::string& text, const bool is_title = false );

	virtual void Create();
	virtual void Destroy();

	const std::string& GetText() const;

protected:

	bool OnMouseDown( const UIEvent::event_data_t* data );
	
	MenuBlock* m_menu = nullptr;
	const std::string m_text = "";
	const bool m_is_title = false;
	
	Label* m_label = nullptr;
	
	std::vector< UIObject* > m_parts = {};
};

}
}

#include "MenuBlock.h"
