#pragma once

#include "ui/object/UIContainer.h"
#include "ui/object/Surface.h"
#include "ui/object/Label.h"

#include <vector>

using namespace std;
using namespace ui::object;
using namespace ui::event;

namespace task {
namespace mainmenu {

class Menu;
	
CHILD_CLASS( MenuItem, UIContainer )
	MenuItem( Menu* menu, const string& text );

	virtual void Create();
	virtual void Destroy();


protected:

	void OnMouseOver( const UIEvent::event_data_t* data );
	void OnMouseOut( const UIEvent::event_data_t* data );
	void OnMouseDown( const UIEvent::event_data_t* data );
	
	Menu* m_menu;
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