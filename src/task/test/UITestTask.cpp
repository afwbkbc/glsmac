#include "UITestTask.h"

#include "engine/Engine.h"

namespace task {
namespace test {

using namespace ui::object;

#define RANDNUM ((float)rand() / (float)RAND_MAX)

void UITestTask::Start() {
	m_panel = new Panel;

	m_panel->SetBackgroundColor( types::Color( 0.0, 1.0, 0.0, 0.3 ) );

	m_panel->SetLeft( 20 );
	m_panel->SetRight( 20 );
	m_panel->SetBottom( 30 );
	m_panel->SetHeight( 130 );

	m_panel->SetAlign( UIObject::ALIGN_BOTTOM );

	m_sub_panel = new ui::object::Panel;
	m_sub_panel->SetBackgroundColor( types::Color( 1.0, 0.0, 0.0, 0.6 ) );

	m_sub_panel->SetWidth( 200 );
	m_sub_panel->SetHeight( 100 );

	m_sub_panel->SetRight( 60 );
	m_sub_panel->SetVAlign( UIObject::ALIGN_CENTER );
	m_sub_panel->SetHAlign( UIObject::ALIGN_RIGHT );

	m_sub_panel->SetBorderWidth( 2 );

	m_panel->AddChild( m_sub_panel );

	g_engine->GetUI()->AddObject( m_panel );

	m_panel->SetOverflow( UIObject::OVERFLOW_HIDDEN );


	m_button = new Button;
	m_sub_panel->AddChild( m_button );
	m_button->SetWidth( 80 );
	m_button->SetHeight( 30 );
	m_button->SetHAlign( UIObject::ALIGN_CENTER );
	m_button->SetTop( 20 );

	m_sub_panel->SetBorderColor( types::Color::WHITE() );
}

void UITestTask::Stop() {
	g_engine->GetUI()->RemoveObject( m_panel );

}

void UITestTask::Iterate() {

	/*if ( rand() % 30 == 0 ) {
		m_panel->SetZIndex( RANDNUM );
		m_panel->SetCoords( ui::UI::vertex_t( rand()%320, rand()%240 ), ui::UI::vertex_t( rand()%320 + 320, rand()%240 + 240 ) );
		m_panel->SetBackgroundColor( types::Color( RANDNUM, RANDNUM, RANDNUM ,RANDNUM ) );
	}*/
}

} /* namespace test */
} /* namespace task */
