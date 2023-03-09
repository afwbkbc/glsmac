#pragma once

#include "MiddleAreaPage.h"

#include "ui/object/Section.h"

namespace game {
namespace world {
	
namespace ui {
	
CLASS( Messages, MiddleAreaPage )
	
	Messages( World* world ) : MiddleAreaPage( world, "Messages" ) {}

	void Create();
	void Destroy();
	void Align();
	
private:
	object::Section* m_messages_list = nullptr;
	
};

}
}
}
