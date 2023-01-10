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

	m_panel = new Panel;
	
	m_menu_item_top = g_engine->GetTextureLoader()->LoadTexture( "console_x.pcx", 717, 1, 731, 311, TextureLoader::LT_ROTATE | TextureLoader::LT_FLIPV );
	
	test = new MenuItem( m_menu_item_top );
	
	m_panel->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	m_panel->SetWidth(400);
	m_panel->SetHeight(600);
	
	m_panel->SetBackgroundColor(types::Color::TRANSPARENT());
	
	m_panel->AddChild(test);
	
	g_engine->GetUI()->AddObject( m_panel );
}

void MainMenuTask::Stop() {
	
	m_panel->RemoveChild(test);
	
	g_engine->GetUI()->RemoveObject( m_panel );
	g_engine->GetUI()->RemoveObject( m_background );
	
	delete m_menu_item_top;
}

void MainMenuTask::Iterate() {

}

} /* namespace mainmenu */
} /* namespace task */
