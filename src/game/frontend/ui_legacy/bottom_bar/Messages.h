#pragma once

#include "MiddleAreaPage.h"

#include "util/Timer.h"

namespace ui_legacy::object {
class Label;
class TextView;
}

namespace game {
namespace frontend {
namespace ui_legacy {

CLASS( Messages, MiddleAreaPage )

	Messages( Game* game )
		: MiddleAreaPage( game, "Messages" ) {}

	void Create() override;
	void Iterate() override;
	void Destroy() override;
	void Align() override;

	void AddMessage( const std::string& text );

private:
	::ui_legacy::object::TextView* m_messages_list = nullptr;

	// for testing
	util::Timer m_test_timer;
};

}
}
}
