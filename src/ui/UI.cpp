#include "UI.h"

#include "engine/Engine.h"

namespace ui {

void UI::Start() {
	Log( "Creating UI" );

	m_shape_scene = new scene::Scene( "UIShape", scene::SCENE_TYPE_ORTHO );
	g_engine->GetRenderer()->AddScene( m_shape_scene );

	m_text_scene = new scene::Scene( "UIText", scene::SCENE_TYPE_TEXT );
	g_engine->GetRenderer()->AddScene( m_text_scene );

	m_root_object.Create();
	m_root_object.UpdateObjectArea();

	m_clamp.x.SetRange( 0.0, g_engine->GetRenderer()->GetWindowWidth(), -1.0, 1.0 );
	m_clamp.y.SetRange( 0.0, g_engine->GetRenderer()->GetWindowHeight(), -1.0, 1.0 );
	m_clamp.y.SetInversed( true );
}

void UI::Stop() {
	Log( "Destroying UI" );

	m_root_object.Destroy();

	g_engine->GetRenderer()->RemoveScene( m_text_scene );
	delete m_text_scene;

	g_engine->GetRenderer()->RemoveScene( m_shape_scene );
	delete m_shape_scene;
}

void UI::Resize() {
	Log( "Resizing UI" );
	m_clamp.x.SetSrcRange( 0.0, g_engine->GetRenderer()->GetWindowWidth() );
	m_clamp.y.SetSrcRange( 0.0, g_engine->GetRenderer()->GetWindowHeight() );
	m_root_object.Realign();
}

void UI::Iterate() {

}

} /* namespace ui */
