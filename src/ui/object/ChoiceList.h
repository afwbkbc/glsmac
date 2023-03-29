#pragma once

/* List with multiple selectable choices */

#include "UIContainer.h"
#include "Button.h"

namespace ui {
namespace object {

CLASS( ChoiceList, UIContainer )

	typedef std::vector< std::string > choices_t;
	
	ChoiceList( const std::string& class_name = "" );

	void SetImmediateMode( const bool immediate_mode );
	
	void SetChoices( const choices_t& choices );
	void SetValue( const std::string& value );
	const std::string& GetValue() const;

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
	
	bool m_immediate_mode = true;
	
	choices_t m_choices = {};
	const std::string m_empty_choice = "";
	std::unordered_map< std::string, Button* > m_buttons = {};
	ssize_t m_value = -1;
	std::unordered_map< Button*, size_t > m_button_values = {};
	
	void SetActiveButton( Button* button );
	
	const std::vector< Style::attribute_type_t > m_forwarded_style_attributes = {
		Style::A_TEXTURE_BACK,
		Style::A_TEXTURE_LEFT,
		Style::A_TEXTURE_TOP,
		Style::A_TEXTURE_RIGHT,
		Style::A_TEXTURE_BOTTOM,
		Style::A_TEXTURE_LEFTTOP,
		Style::A_TEXTURE_RIGHTTOP,
		Style::A_TEXTURE_RIGHTBOTTOM,
		Style::A_TEXTURE_LEFTBOTTOM,
		Style::A_FONT,
		Style::A_TEXT_COLOR,
		Style::A_TEXT_ALIGN,
		Style::A_BUTTON_CLICK_SOUND,
		Style::A_BUTTON_MOVE_SOUND,
	};
	
	void SelectChoice();
};

} /* namespace object */
} /* namespace ui */
