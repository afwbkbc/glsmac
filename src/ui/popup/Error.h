#pragma once

#include "ui/object/Popup.h"

#include "ui/object/Section.h"
#include "ui/object/Label.h"
#include "ui/object/Button.h"

namespace ui {
namespace popup {

CLASS( Error, object::Popup )

	Error( const std::string class_name = "Popup" );

	void Create() override;
	void Destroy() override;

	void SetText( const std::string& text );
	
private:
	std::string m_text = "";
	
	Section* m_body = nullptr;
	
	object::Label* m_label = nullptr;
	object::Button* m_ok_button = nullptr;
	
};

}
}
