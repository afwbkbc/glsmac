#pragma once

#include "base/Base.h"

namespace ui {

class UI;
namespace event {
class UIEvent;
}

namespace module {

CLASS( Module, base::Base )

	Module( UI* ui );

	virtual void ProcessEvent( event::UIEvent* event ) {}

protected:
	void Activate();
	void Deactivate();

	bool m_is_active = false;
	UI* m_ui = nullptr;

};

}
}
