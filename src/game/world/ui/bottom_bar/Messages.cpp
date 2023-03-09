#include "Messages.h"

namespace game {
namespace world {
namespace ui {

void Messages::Create() {
	MiddleAreaPage::Create();
	
	NEW( m_messages_list, object::Section, "MapBottomBarMiddleAreaMessagesList" )
	AddChild( m_messages_list );
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
