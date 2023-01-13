#include "MainMenu.h"

#include <cmath>
#include <algorithm>

#include "engine/Engine.h"

#include "scene/actor/TextActor.h"

#include "menu/Main.h"

using namespace std;
using namespace ui::object;
using namespace loader::texture;

namespace game {
namespace mainmenu {

void MainMenu::Start() {
	
	// background
	m_background = new Image( "xopeningb.pcx" );
	m_background->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	m_background->SetZIndex( 0.5 );
	g_engine->GetUI()->AddObject( m_background );

	// menu entries	
	m_menu_item_textures = {
		// UPPER TOP
		g_engine->GetTextureLoader()->LoadTexture( "console_x.pcx", 708, 1, 715, 354, TextureLoader::LT_ROTATE | TextureLoader::LT_FLIPV | TextureLoader::LT_CONTRAST, 1.5 ),
		// LOWER TOP
		g_engine->GetTextureLoader()->LoadTexture( "console_x.pcx", 708, 1, 715, 354, TextureLoader::LT_ROTATE | TextureLoader::LT_FLIPV | TextureLoader::LT_CONTRAST, 0.8 ),
		// BOTTOM
		g_engine->GetTextureLoader()->LoadTexture( "console_x.pcx", 699, 1, 706, 293, TextureLoader::LT_ROTATE | TextureLoader::LT_FLIPV | TextureLoader::LT_CONTRAST, 0.2 ),
		// LEFT
		g_engine->GetTextureLoader()->LoadTexture( "interface.pcx", 472, 756, 476, 790, TextureLoader::LT_CONTRAST, 0.3 ),
		// RIGHT
		g_engine->GetTextureLoader()->LoadTexture( "interface.pcx", 299, 756, 303, 790, TextureLoader::LT_CONTRAST, 0.2 ),
		// BODY
		g_engine->GetTextureLoader()->LoadTexture( "palette.pcx", 450, 28, 450, 28, TextureLoader::LT_ALPHA, 0.8 )
	};
	
	m_menu_item_font = g_engine->GetFontLoader()->LoadFont( "arialnb.ttf", 20 );
	
	ShowMenu( new Main( this ) );
}

void MainMenu::GoBack() {
	if ( !m_menu_history.empty() ) {
		m_menu_object->Hide();
		delete m_menu_object;
		m_menu_object = m_menu_history.back();
		m_menu_history.pop_back();
		m_menu_object->Show();
	}
	else {
		g_engine->ShutDown();
	}
}

void MainMenu::Stop() {
	
	if ( m_menu_object ) {
		m_menu_object->Hide();
		delete m_menu_object;
		
		for (auto& it : m_menu_history) {
			delete it;
		}
	}
	
	g_engine->GetUI()->RemoveObject( m_background );
	
	for (auto& t : m_menu_item_textures) {
		delete t;
	}
}

void MainMenu::ShowMenu( MenuObject* menu_object ) {
	if ( m_menu_object ) {
		m_menu_object->Hide();
		m_menu_history.push_back( m_menu_object );
	}
	m_menu_object = menu_object;
	m_menu_object->Show();
}


} /* namespace mainmenu */
} /* namespace game */
