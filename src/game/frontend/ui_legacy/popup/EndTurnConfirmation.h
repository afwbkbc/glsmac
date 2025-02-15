#pragma once

#include "Popup.h"

#include "ui_legacy/Types.h"

// TODO: remove this
#include "ui_legacy/object/ChoiceList.h"

namespace ui_legacy::object {
class Label;
class Section;
class SoundEffect;
}

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {

CLASS( EndTurnConfirmation, Popup )

	EndTurnConfirmation( Game* game, const ::ui_legacy::ui_handler_t on_confirm );

	void Create() override;
	void Destroy() override;
	void Align() override;

private:

	const ::ui_legacy::ui_handler_t m_on_confirm;

	::ui_legacy::object::Section* m_message_section = nullptr;
	struct {
		::ui_legacy::object::Label* first = nullptr;
		::ui_legacy::object::Label* second = nullptr;
	} m_message_labels = {};
	::ui_legacy::object::NumChoiceList* m_choices = nullptr;

	void SelectChoice();
};

}
}
}
}
