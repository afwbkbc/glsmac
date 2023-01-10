#include <cmath>

#include "MainMenuTask.h"

#include "engine/Engine.h"

using namespace ui::object;
using namespace loader::texture;

namespace task {
namespace mainmenu {

static MenuItem *test;

#define RANDNUM ( (float)rand() / (float)RAND_MAX )

void MainMenuTask::Start() {
	
	m_background = new Image( "xopeningb.pcx" );
	m_background->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	m_background->SetZIndex( 0.5 );
	g_engine->GetUI()->AddObject( m_background );

	// TOP
	m_menu_item_textures = {
		// TOP
		//g_engine->GetTextureLoader()->LoadTexture( "console_x.pcx", 717, 1, 731, 311, TextureLoader::LT_ROTATE | TextureLoader::LT_FLIPV | TextureLoader::LT_CONTRAST, 0.7 ),
		g_engine->GetTextureLoader()->LoadTexture( "console_x.pcx", 708, 1, 715, 311, TextureLoader::LT_ROTATE | TextureLoader::LT_FLIPV | TextureLoader::LT_CONTRAST, 0.7 ),
		// BOTTOM
		g_engine->GetTextureLoader()->LoadTexture( "console_x.pcx", 699, 1, 706, 240, TextureLoader::LT_ROTATE | TextureLoader::LT_FLIPV | TextureLoader::LT_CONTRAST, 0.7 ),
		// LEFT
		g_engine->GetTextureLoader()->LoadTexture( "interface.pcx", 472, 756, 476, 799, TextureLoader::LT_CONTRAST, 0.6 ),
		// RIGHT
		g_engine->GetTextureLoader()->LoadTexture( "interface.pcx", 299, 756, 303, 799, TextureLoader::LT_CONTRAST, 0.5 ),
		// BODY
		g_engine->GetTextureLoader()->LoadTexture( "palette.pcx", 480, 28, 480, 28, TextureLoader::LT_ALPHA, 0.9 )
		// TOP SHADOW
		//g_engine->GetTextureLoader()->LoadTexture( "console_x.pcx", 683, 1, 697, 311, TextureLoader::LT_ROTATE | TextureLoader::LT_FLIPV )
	};
	
	test = new MenuItem( &m_menu_item_textures );
	
	m_panel = new Panel;
	
	m_panel->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	m_panel->SetWidth(160);
	m_panel->SetHeight(200);
	
	m_panel->SetBackgroundColor(types::Color::TRANSPARENT());
	
	m_panel->AddChild(test);
	
	g_engine->GetUI()->AddObject( m_panel );
}

void MainMenuTask::Stop() {
	
	m_panel->RemoveChild(test);
	
	g_engine->GetUI()->RemoveObject( m_panel );
	g_engine->GetUI()->RemoveObject( m_background );
	
	for (auto& t : m_menu_item_textures) {
		delete t;
	}
}

void MainMenuTask::Iterate() {

}

} /* namespace mainmenu */
} /* namespace task */
