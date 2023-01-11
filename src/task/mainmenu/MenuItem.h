#pragma once

#include "ui/object/UIContainer.h"
#include "ui/object/Surface.h"
#include "ui/object/Label.h"

#include <vector>

#include "MenuItemMesh.h"

using namespace std;
using namespace ui::object;
using namespace ui::event;

namespace task {
namespace mainmenu {

class Menu;
	
CHILD_CLASS( MenuItem, UIContainer )
	MenuItem( const Menu* menu, const string& text );

	virtual void Create();
	virtual void Destroy();


protected:

	void OnMouseOver( const UIEvent::event_data_t* data );
	void OnMouseOut( const UIEvent::event_data_t* data );
	
	const Menu* m_menu;
	const string m_text;
	
	Surface* m_upper_top;
	Surface* m_lower_top;
	Surface* m_bottom;
	Surface* m_left;
	Surface* m_right;
	Surface* m_body;
	Label* m_label;
	
};

}
}

#include "Menu.h"