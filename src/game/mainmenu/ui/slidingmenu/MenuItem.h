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
	
	MenuBlock* m_menu = nullptr;
	const string m_text = "";
	const bool m_is_title = false;
	
	Surface* m_upper_top = nullptr;
	Surface* m_lower_top = nullptr;
	Surface* m_bottom = nullptr;
	Surface* m_left = nullptr;
	Surface* m_right = nullptr;
	Surface* m_body = nullptr;
	Label* m_label = nullptr;
	
};

}
}

#include "MenuBlock.h"