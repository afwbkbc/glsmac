#include "Section.h"

#include "engine/Engine.h"

#include "ui/object/Section.h"

namespace task {
namespace game {
namespace ui {

Section::Section( Game* game, const std::string class_name, const std::string section_class_prefix )
	: UI( game, class_name )
	, m_section_class_prefix( section_class_prefix ) {
	//
}

void Section::Create() {
	UI::Create();

	NEW( m_outer, ::ui::object::Section, m_config.no_outer_border
		? ""
		: m_section_class_prefix + "SectionOuter" );
	UI::AddChild( m_outer );

	NEW( m_inner, ::ui::object::Section, m_config.no_inner_border
		? ""
		: m_section_class_prefix + "SectionInner" );
	if ( !m_title_text.empty() ) {
		m_inner->SetTitleText( m_title_text );
	}
	m_outer->AddChild( m_inner );

}

void Section::Destroy() {

	m_outer->RemoveChild( m_inner );
	UI::RemoveChild( m_outer );

	UI::Destroy();
}

void Section::AddChild( ::ui::object::UIObject* object ) {
	m_inner->AddChild( object );
}

void Section::RemoveChild( ::ui::object::UIObject* object ) {
	m_inner->RemoveChild( object );
}

void Section::SetTitleText( const std::string& title_text ) {
	if ( m_title_text != title_text ) {
		m_title_text = title_text;
		if ( m_inner ) {
			m_inner->SetTitleText( m_title_text );
		}
	}
}

}
}
}
