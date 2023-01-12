#include "Label.h"

#include "engine/Engine.h"

using namespace types;

namespace ui {
namespace object {

Label::Label(Font* font, const string& text, const Color& color)
	: UIObject()
	, m_font( font )
	, m_text( text )
	, m_color( color )
{
	SetAlign( ALIGN_CENTER );
}

void Label::SetTextColor( const Color& color ) {
	m_color = color;
	m_actor->SetColor( color );
}

void Label::Create() {
	UIObject::Create();
	
	m_actor = new scene::actor::TextActor( m_font, m_text, m_color );
	
	g_engine->GetUI()->GetTextScene()->AddActor(m_actor);

}

void Label::Destroy() {

	g_engine->GetUI()->GetTextScene()->RemoveActor(m_actor);
	delete m_actor;
	
	UIObject::Destroy();
}

void Label::Align() {
	UIObject::Align();

	size_t xpos = 0;
	size_t ypos = 0;

	if ( ( m_align & ALIGN_HCENTER ) == ALIGN_HCENTER ) {
		xpos = ( m_object_area.left + m_object_area.right ) / 2 - m_actor->GetFont()->GetTextWidth(m_text.c_str()) / 2;
	}
	else if ( m_align & ALIGN_LEFT ) {
		xpos = m_object_area.left;
	}
	else if ( m_align & ALIGN_RIGHT ) {
		xpos = m_object_area.right - m_actor->GetFont()->GetTextWidth(m_text.c_str());
	}
	
	if ( ( m_align & ALIGN_VCENTER ) == ALIGN_VCENTER ) {
		ypos = ( m_object_area.top + m_object_area.bottom ) / 2 + m_actor->GetFont()->GetTextHeight(m_text.c_str()) / 2;
	}
	else if ( m_align & ALIGN_TOP ) {
		ypos = m_object_area.top + m_actor->GetFont()->GetTextHeight(m_text.c_str());
	}
	else if ( m_align & ALIGN_BOTTOM ) {
		ypos = m_object_area.bottom;
	}
	
	m_actor->SetPosition({
		ClampX( xpos ),
		ClampY( ypos ),
		-1.0,
	});
}

void Label::Draw() {
	UIObject::Draw();
}


}
}
