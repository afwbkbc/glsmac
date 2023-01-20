#pragma once

/* List with multiple selectable choices */

#include "UIContainer.h"
#include "Button.h"

namespace ui {
namespace object {

CHILD_CLASS( ChoiceList, UIContainer )

	typedef vector< string > choices_t;
	
	ChoiceList( const string& class_name = "" );

	void SetChoices( const choices_t& choices );
	void SetValue( const string& value );
	const string& GetValue() const;

	virtual void Create();
	virtual void Destroy();
	
	//void OnChange( UIEventHandler::handler_function_t func );
	
protected:
	virtual void ApplyStyle();
	
	bool OnKeyDown( const UIEvent::event_data_t* data );
	bool OnKeyUp( const UIEvent::event_data_t* data );
	bool OnKeyPress( const UIEvent::event_data_t* data );
	
	
	
private:
	void UpdateButtons();
	
	choices_t m_choices = {};
	unordered_map< string, Button* > m_buttons = {};
	size_t m_value = 0;
	unordered_map< Button*, size_t > m_button_values = {};
	
	void SetActiveButton( Button* button );
	
	const vector< Style::attribute_type_t > m_forwarded_style_attributes = {
		Style::A_TEXTURE_BACK,
		Style::A_TEXTURE_BORDER_LEFT,
		Style::A_TEXTURE_BORDER_TOP,
		Style::A_TEXTURE_BORDER_RIGHT,
		Style::A_TEXTURE_BORDER_BOTTOM,
		Style::A_TEXTURE_BORDER_CORNER_LT,
		Style::A_TEXTURE_BORDER_CORNER_RT,
		Style::A_TEXTURE_BORDER_CORNER_RB,
		Style::A_TEXTURE_BORDER_CORNER_LB,
		Style::A_FONT,
		Style::A_TEXTCOLOR,
		Style::A_TEXTALIGN,
	};
};

} /* namespace object */
} /* namespace ui */
