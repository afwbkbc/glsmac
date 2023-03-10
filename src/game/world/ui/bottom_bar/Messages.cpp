#include "Messages.h"

#include "engine/Engine.h"

namespace game {
namespace world {
namespace ui {

void Messages::Create() {
	MiddleAreaPage::Create();
	
	NEW( m_messages_list, object::ScrollView, "MapBottomBarMiddleAreaMessagesList" );
	
	// for testing
/*	auto* test_font = g_engine->GetFontLoader()->LoadFont( "arialn.ttf", 16 );
	for ( auto i = 1 ; i <= 8 ; i++ ) {
		NEWV( label, object::Label );
		label->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		label->SetFont( test_font );
		label->SetTextColor( Color::FromRGB( 255, 255, 255 ) );
		label->SetText( "SCROLLVIEW LINE " + std::to_string( i ) );
		label->SetLeft( 4 );
		label->SetTop( 4 + ( i - 1 ) * 17 );
		label->SetHeight( 17 );
		m_messages_labels.push_back( label );
		m_messages_list->AddChild( label );
	}*/
	
	AddChild( m_messages_list );
}

void Messages::Destroy() {
	
	for ( auto& label : m_messages_labels ) {
		m_messages_list->RemoveChild( label );
	}
	m_messages_labels.clear();
	
	RemoveChild( m_messages_list );
	
	MiddleAreaPage::Destroy();
}

void Messages::Align() {
	MiddleAreaPage::Align();
	
	
}

}
}
}
