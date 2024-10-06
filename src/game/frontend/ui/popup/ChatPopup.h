#pragma once

#include "OkCancelPopup.h"

namespace ui::object {
class Section;
class Label;
class Input;
}

namespace game {
namespace frontend {
namespace ui {
namespace popup {

CLASS( ChatPopup, OkCancelPopup )

	ChatPopup( Game* game );

	void Create() override;
	void Destroy() override;

protected:
	void OnOk() override;

private:

	::ui::object::Section* m_message_section = nullptr;
	::ui::object::Label* m_message_label = nullptr;
	::ui::object::Input* m_message_input = nullptr;
	struct {

	} m_buttons;

};

}
}
}
}
