#include "FontTestTask.h"

#include "engine/Engine.h"

#include "scene/actor/TextActor.h"

namespace task {
namespace test {

void FontTestTask::Start() {
	m_scene = new scene::Scene( "FontTest", scene::SCENE_TYPE_TEXT );
	m_font1 = g_engine->GetFontLoader()->LoadFont( "default.ttf", 36 );
	m_font2 = g_engine->GetFontLoader()->LoadFont( "default.ttf", 18 );
	m_text1 = new scene::actor::TextActor( m_font1, "TEXT1" );
	m_text1->SetPosition( scene::actor::Actor::vec2_t( -0.7, 0.4 ) );
	m_scene->AddActor( m_text1 );
	m_text2 = new scene::actor::TextActor( m_font2, "TEXT2", types::Color::RED() );
	m_text2->SetPosition( scene::actor::Actor::vec3_t( -0.9, -0.6, 0.5 ) );
	m_scene->AddActor( m_text2 );
	m_text3 = new scene::actor::TextActor( m_font2, "TEXT3", types::Color::GREEN() );
	m_text3->SetPosition( scene::actor::Actor::vec3_t( 0.4, -0.4, 0.7 ) );
	m_scene->AddActor( m_text3 );
	g_engine->GetRenderer()->AddScene( m_scene );
}

void FontTestTask::Stop() {
	g_engine->GetRenderer()->RemoveScene( m_scene );
	delete m_text1;
	delete m_text2;
	delete m_text3;
	delete m_font1;
	delete m_font2;
	delete m_scene;
}

void FontTestTask::Iterate() {

}


} /* namespace test */
} /* namespace task */
