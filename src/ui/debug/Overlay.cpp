#include "Overlay.h"

#include "engine/Engine.h"

using namespace std;

namespace ui {
namespace debug {

void Overlay::Create() {
	UIContainer::Create();
	
	m_fps_counter_font = g_engine->GetFontLoader()->LoadFont( "arialn.ttf", 18 );
	m_fps_counter = new Label( "" );
	m_fps_counter->SetFont( m_fps_counter_font );
	m_fps_counter->SetTextColor( { 1.0, 1.0, 1.0, 0.5 } );
	m_fps_counter->SetAlign( UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
	m_fps_counter->SetLeft( 3 );
	g_engine->GetUI()->AddObject( m_fps_counter );
	
	m_fps_timer.SetInterval( 1000 ); // update statistics every 1 second
}

void Overlay::Destroy() {
	
	m_fps_timer.Stop();
	
	g_engine->GetUI()->RemoveObject( m_fps_counter );
	
	UIContainer::Destroy();
}

void Overlay::Iterate() {
	UIContainer::Iterate();
	
	m_fps_frames++;
	if (m_fps_timer.Ticked()) {
		m_fps_counter->SetText( "FPS: " + to_string( m_fps_frames ) + " ( LIMIT = " + to_string(g_max_fps) + " )" );
		m_fps_frames = 0;
	}
}	

}
}
