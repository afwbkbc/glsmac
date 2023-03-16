#include "Messages.h"

#include "engine/Engine.h"

namespace game {
namespace world {
namespace ui {

void Messages::Create() {
	MiddleAreaPage::Create();
	
	NEW( m_messages_list, object::TextView, "MapBottomBarMiddleAreaMessagesList" );
		m_messages_list->SetLinesLimit( 20 );
	AddChild( m_messages_list );

	// for testing
	m_test_timer.SetInterval( 200 );
	/*for ( auto i = 1 ; i <= 15 ; i++ ) {
		m_messages_list->AddLine( "SCROLLVIEW TEST LINE " + std::to_string( i ) );
	}*/
	

}

void Messages::Iterate() {
	MiddleAreaPage::Iterate();
	
	// for testing
	while ( m_test_timer.HasTicked () ) {
		m_messages_list->AddLine( "SCROLLVIEW TEST LINE " + std::to_string( rand() % 99999999 ) );
	}

}

void Messages::Destroy() {
	
	RemoveChild( m_messages_list );
	
	MiddleAreaPage::Destroy();
}

void Messages::Align() {
	MiddleAreaPage::Align();
	
	
}

}
}
}
