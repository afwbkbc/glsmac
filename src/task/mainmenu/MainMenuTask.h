#pragma once

#include "../Task.h"

#include <vector>
#include <string>

#include "ui/object/Image.h"
#include "types/Texture.h"
#include "types/Font.h"
#include "scene/actor/Actor.h"
#include "Menu.h"
#include "MenuItem.h"

using namespace std;
using namespace types;

namespace task {
namespace mainmenu {

MINOR_CLASS( MainMenu, Task )
	void Start();
	void Stop();
	void Iterate();
	
protected:
	friend class Menu;
	friend class MenuItem;

	void OnMenuClick( const string& choice );
	
	ui::object::Image *m_background = nullptr;
	
	vector<const types::Texture*> m_menu_item_textures;
	types::Font *m_menu_item_font;
	const Color m_menu_item_text_color_normal = Color::FromRGBA( 94, 162 ,137, 255 );
	const Color m_menu_item_text_color_highlight = Color::FromRGBA( 193, 240 ,204, 255 );
	
	Menu* m_menu = nullptr;
	
	vector<string> m_menu_choices = {};

private:
	void ShowMenu( const vector<string>& choices = {} );
	
	void ShowNextMenu();
	
};

} /* namespace mainmenu */
} /* namespace task */
