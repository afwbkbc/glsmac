#pragma once

#include "MiddleAreaPage.h"

#include "ui/object/ScrollView.h"
#include "ui/object/Label.h"

namespace game {
namespace world {
	
namespace ui {
	
CLASS( Messages, MiddleAreaPage )
	
	Messages( World* world ) : MiddleAreaPage( world, "Messages" ) {}

	void Create();
	void Destroy();
	void Align();
	
private:
	object::ScrollView* m_messages_list = nullptr;
	std::vector< object::Label* > m_messages_labels = {};
	
};

}
}
}
