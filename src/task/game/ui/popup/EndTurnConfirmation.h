#pragma once

#include "Popup.h"

#include "ui/object/Label.h"
#include "ui/object/ChoiceList.h"
#include "ui/object/SoundEffect.h"

#include "../../../../ui/UI.h"

namespace task {
namespace game {
namespace ui {
namespace popup {

CLASS( EndTurnConfirmation, Popup )

	EndTurnConfirmation( Game* game, const ui_handler_t on_confirm );

	void Create() override;
	void Destroy() override;
	void Align() override;

private:

	const ui_handler_t m_on_confirm;

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
