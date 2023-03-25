#pragma once

#include "MiddleAreaPage.h"

#include "ui/object/TextView.h"
#include "ui/object/Label.h"

#include "util/Timer.h"

namespace game {
namespace world {
namespace ui {
	
CLASS( Messages, MiddleAreaPage )
	
	Messages( World* world ) : MiddleAreaPage( world, "Messages" ) {}

	void Create();
	void Iterate();
	void Destroy();
	void Align();
	
private:
	object::TextView* m_messages_list = nullptr;
	
	// for testing
	util::Timer m_test_timer;
};

}
}
}
