#pragma once

#include "Popup.h"

#include "ui/object/Surface.h"
#include "ui/object/Label.h"
#include "ui/object/ChoiceList.h"
#include "ui/object/SoundEffect.h"

#include "../../../../ui/UI.h"

namespace task {
namespace game {
namespace ui {
namespace popup {

CLASS( PleaseDontGo, Popup )

	PleaseDontGo( Game* game, const ui_handler_t on_order );

	void Create() override;
	void Destroy() override;

private:

	const ui_handler_t m_on_order;

	::ui::object::Surface* m_image = nullptr;
	::ui::object::Label* m_text = nullptr;
	::ui::object::NumChoiceList* m_choices = nullptr;
	::ui::object::SoundEffect* m_sound = nullptr;

	void SelectChoice();
};

}
}
}
}
