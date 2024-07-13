#pragma once

#include "Popup.h"

namespace ui::object {
class Button;
}

namespace game {
namespace frontend {
namespace ui {
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
		::ui::object::Button* ok = nullptr;
		::ui::object::Button* cancel = nullptr;
	} m_buttons;

};

}
}
}
}
