#pragma once

#include "Popup.h"

#include "ui/Types.h"

// TODO: remove this
#include "ui/object/ChoiceList.h"

namespace ui::object {
class Surface;
class Label;
class SoundEffect;
}

namespace task {
namespace game {
namespace ui {
namespace popup {

CLASS( PleaseDontGo, Popup )

	PleaseDontGo( Game* game, const ::ui::ui_handler_t on_order );

	void Create() override;
	void Destroy() override;

private:

	const ::ui::ui_handler_t m_on_order;

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
