#pragma once

#include <map>
#include <vector>

/* List with multiple selectable choices */

#include "UIContainer.h"
#include "Button.h"

namespace ui {
namespace object {

CLASS( ChoiceList, UIContainer )

	typedef size_t value_t;
	typedef std::vector< std::pair< value_t, std::string > > choices_t;
	
	ChoiceList( const std::string& class_name = "" );

	void SetImmediateMode( const bool immediate_mode );
	
	void SetChoices( const choices_t& choices );
	void SetValue( const value_t value );
	const value_t GetValue() const;
	
	// TODO: refactor and remove
	void SetValueString( const std::string& choice );
	const std::string& GetValueString() const;
	void SetChoicesV( const std::vector< std::string >& labels );

	virtual void Create();
	virtual void Destroy();
	virtual void Align();
	
	//void OnChange( UIEventHandler::handler_function_t func );
	
	void SetItemMargin( const coord_t item_margin );
	void SetItemHeight( const coord_t item_height );
	
protected:
	virtual void ApplyStyle();
	
	bool OnKeyDown( const UIEvent::event_data_t* data );
	bool OnKeyUp( const UIEvent::event_data_t* data );
	bool OnKeyPress( const UIEvent::event_data_t* data );
	
	
	
private:
	void UpdateButtons();
	
	bool m_immediate_mode = true;
	
	struct {
		coord_t margin = 0;
		coord_t height = 0;
	} m_item_align = {};
	
	std::vector< value_t > m_values = {};
	std::map< value_t, std::string > m_labels = {};
	const std::string m_empty_choice = "";
	std::unordered_map< value_t, Button* > m_buttons = {};
	ssize_t m_value = -1;
	size_t m_value_index = 0;
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
		Style::A_BORDER_SIZE,
	};
	
	void SelectChoice();
};

} /* namespace object */
} /* namespace ui */
