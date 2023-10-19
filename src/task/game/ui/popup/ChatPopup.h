#pragma once

#include "Popup.h"

#include "ui/object/Label.h"

namespace task {
namespace game {
namespace ui {
namespace popup {

CLASS( ChatPopup, Popup )

	ChatPopup( Game* game );

	void Create() override;
	void Destroy() override;

private:

	::ui::object::Label* m_text = nullptr;

};

}
}
}
}
