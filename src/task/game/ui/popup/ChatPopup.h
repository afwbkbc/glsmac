#pragma once

#include "OkCancelPopup.h"

#include "ui/object/Section.h"
#include "ui/object/Label.h"
#include "ui/object/Input.h"

namespace task {
namespace game {
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
