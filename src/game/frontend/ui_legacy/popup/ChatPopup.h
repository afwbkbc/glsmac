#pragma once

#include "OkCancelPopup.h"

namespace ui_legacy::object {
class Section;
class Label;
class Input;
}

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {

CLASS( ChatPopup, OkCancelPopup )

	ChatPopup( Game* game );

	void Create() override;
	void Destroy() override;

protected:
	void OnOk() override;

private:

	::ui_legacy::object::Section* m_message_section = nullptr;
	::ui_legacy::object::Label* m_message_label = nullptr;
	::ui_legacy::object::Input* m_message_input = nullptr;

};

}
}
}
}
