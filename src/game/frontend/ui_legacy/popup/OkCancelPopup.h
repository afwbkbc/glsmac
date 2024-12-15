#pragma once

#include "Popup.h"

namespace ui_legacy::object {
class Button;
}

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {

CLASS( OkCancelPopup, Popup )

	OkCancelPopup( Game* game );

	void Create() override;
	void Align() override;
	void Destroy() override;

protected:
	virtual void OnOk() {};

private:
	struct {
		::ui_legacy::object::Button* ok = nullptr;
		::ui_legacy::object::Button* cancel = nullptr;
	} m_buttons;

};

}
}
}
}
