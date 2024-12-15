#pragma once

#include "ui_legacy/object/Popup.h"

namespace ui_legacy {

namespace object {
class Section;
class Label;
class Button;
}

namespace popup {

CLASS( Error, object::Popup )

	Error( const std::string class_name = "Popup" );

	void Create() override;
	void Destroy() override;

	void SetText( const std::string& text );

private:
	std::string m_text = "";

	object::Section* m_body = nullptr;

	object::Label* m_label = nullptr;
	object::Button* m_ok_button = nullptr;

};

}
}
