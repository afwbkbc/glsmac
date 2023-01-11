#include <cmath>

#include "MainMenuTask.h"

#include "engine/Engine.h"

#include "scene/actor/TextActor.h"

using namespace ui::object;
using namespace loader::texture;

namespace task {
namespace mainmenu {

#define RANDNUM ( (float)rand() / (float)RAND_MAX )

void MainMenuTask::Start() {
	
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
	
	m_menu = new Menu( this );
	m_menu->AddItem("START GAME");
	m_menu->AddItem("QUICK START");
	m_menu->AddItem("SCENARIO");
	m_menu->AddItem("LOAD GAME");
	m_menu->AddItem("MULTIPLAYER");
	m_menu->AddItem("VIEW CREDITS");
	m_menu->AddItem("EXIT GAME");
	
	g_engine->GetUI()->AddObject( m_menu );
	
}

void MainMenuTask::Stop() {
	
	g_engine->GetUI()->RemoveObject( m_menu );
	g_engine->GetUI()->RemoveObject( m_background );
	
	for (auto& t : m_menu_item_textures) {
		delete t;
	}
}

void MainMenuTask::Iterate() {

}

} /* namespace mainmenu */
} /* namespace task */
