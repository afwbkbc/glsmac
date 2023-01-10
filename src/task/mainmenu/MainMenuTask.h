#pragma once

#include "../Task.h"

#include <vector>

#include "types/Texture.h"
#include "ui/object/Image.h"
#include "ui/object/Panel.h"
#include "MenuItem.h"
#include "MenuItemMesh.h"
#include "MenuItemTexture.h"

using namespace std;

namespace task {
namespace mainmenu {

MINOR_CLASS( MainMenu, Task )
	void Start();
	void Stop();
	void Iterate();
	
protected:
	const MenuItemMesh m_menu_item_mesh;
	
	types::Texture* m_menu_item_top;
	types::Texture* m_menu_item_body;
	types::Texture* m_menu_item_bottom;
	
	ui::object::Image *m_background;
	ui::object::Panel *m_panel;

	vector<MenuItem*> m_menu_items;
};

} /* namespace mainmenu */
} /* namespace task */
