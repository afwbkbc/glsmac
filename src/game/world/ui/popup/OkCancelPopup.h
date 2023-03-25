#pragma once

#include "Popup.h"

#include "ui/object/Button.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

CLASS( OkCancelPopup, Popup )
	
	OkCancelPopup( World* world );
	
	void Create();
	void Align();
	void Destroy();

protected:
	virtual void OnOk() {};
	
private:
	struct {
		::ui::object::Button* ok = nullptr;
		::ui::object::Button* cancel = nullptr;
	} m_buttons;
	
};

}
}
}
}
