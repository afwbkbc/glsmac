#pragma once

#include "ui/object/Popup.h"

namespace ui {

namespace object {
class Section;
class Label;
class Button;
}

namespace popup {

CLASS( Confirm, object::Popup )

	Confirm( const std::string class_name = "Popup" );

	void Create() override;
	void Destroy() override;

	void SetText( const std::string& text );

private:

	std::string m_text = "";

	object::Section* m_body = nullptr;

	object::Label* m_label = nullptr;
	struct {
		object::Button* ok = nullptr;
		object::Button* cancel = nullptr;
	} m_buttons;

	void Confirmed();

};

}
}
