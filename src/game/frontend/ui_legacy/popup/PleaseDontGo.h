#pragma once

#include "Popup.h"

#include "ui_legacy/Types.h"

// TODO: remove this
#include "ui_legacy/object/ChoiceList.h"

namespace ui_legacy::object {
class Surface;
class Label;
class SoundEffect;
}

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {

CLASS( PleaseDontGo, Popup )

	PleaseDontGo( Game* game, const ::ui_legacy::ui_handler_t on_order );

	void Create() override;
	void Destroy() override;

private:

	const ::ui_legacy::ui_handler_t m_on_order;

	::ui_legacy::object::Surface* m_image = nullptr;
	::ui_legacy::object::Label* m_text = nullptr;
	::ui_legacy::object::NumChoiceList* m_choices = nullptr;
	::ui_legacy::object::SoundEffect* m_sound = nullptr;

	void SelectChoice();
};

}
}
}
}
