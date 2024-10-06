#pragma once

#include "Popup.h"

#include "ui/Types.h"

// TODO: remove this
#include "ui/object/ChoiceList.h"

namespace ui::object {
class Label;
class Section;
class SoundEffect;
}

namespace game {
namespace frontend {
namespace ui {
namespace popup {

CLASS( EndTurnConfirmation, Popup )

	EndTurnConfirmation( Game* game, const ::ui::ui_handler_t on_confirm );

	void Create() override;
	void Destroy() override;
	void Align() override;

private:

	const ::ui::ui_handler_t m_on_confirm;

	::ui::object::Section* m_message_section = nullptr;
	struct {
		::ui::object::Label* first = nullptr;
		::ui::object::Label* second = nullptr;
	} m_message_labels = {};
	::ui::object::NumChoiceList* m_choices = nullptr;

	void SelectChoice();
};

}
}
}
}
