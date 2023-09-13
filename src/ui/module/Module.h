#pragma once

#include "base/Base.h"

#include "ui/event/UIEvent.h"

namespace ui {
	class UI;
namespace module {
		
CLASS( Module, Base )
	
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

#include "../../ui/UI.h"
