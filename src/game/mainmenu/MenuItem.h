#pragma once

#include "ui/object/UIContainer.h"
#include "ui/object/Surface.h"
#include "ui/object/Label.h"

#include <vector>

using namespace std;
using namespace ui::object;
using namespace ui::event;

namespace game {
namespace mainmenu {

class MenuBlock;
	
CHILD_CLASS( MenuItem, UIContainer )
	MenuItem( MenuBlock* menu, const string& text, const bool is_title = false );

	virtual void Create();
	virtual void Destroy();


protected:

	void OnMouseOver( const UIEvent::event_data_t* data );
	void OnMouseOut( const UIEvent::event_data_t* data );
	void OnMouseDown( const UIEvent::event_data_t* data );
	
	MenuBlock* m_menu;
	const string m_text;
	const bool m_is_title;
	
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

#include "MenuBlock.h"