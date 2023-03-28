#pragma once

#include "ui/object/Popup.h"

#include "ui/object/Section.h"
#include "ui/object/Label.h"
#include "ui/object/Button.h"

namespace ui {
namespace popup {

CLASS( Confirmation, ::ui::object::Popup )

	Confirmation( const std::string class_name = "Popup" );

	void Create();
	void Destroy();

	void SetText( const std::string& text );
	
private:
	
	std::string m_text = "";
	
	Section* m_body = nullptr;
	
	::ui::object::Label* m_label = nullptr;
	struct {
		::ui::object::Button* ok = nullptr;
		::ui::object::Button* cancel = nullptr;
	} m_buttons;

	void Confirm();

};

}
}
