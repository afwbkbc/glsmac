#pragma once

#include <map>
#include <unordered_map>
#include <vector>

/* List with multiple selectable choices */

#include "UIContainer.h"

namespace ui {
namespace object {

class Button;

template< typename KEY_TYPE = size_t >
CLASS( ChoiceList, UIContainer )
	typedef KEY_TYPE value_t;
	typedef std::vector< std::pair< value_t, std::string > > choices_t;

	ChoiceList( const std::string& class_name = "" );

	void SetImmediateMode( const bool immediate_mode );

	void SetChoices( const choices_t& choices );
	void SetValue( const value_t value );
	const value_t GetValue() const;

	// TODO: refactor and remove
	void SetValueString( const std::string& choice, bool allowMissing = false );
	const std::string& GetValueString() const;
	void SetChoicesV( const std::vector< std::string >& labels );

	virtual void Create() override;
	virtual void Destroy() override;
	virtual void Align() override;

	void SetItemMargin( const coord_t item_margin );
	void SetItemHeight( const coord_t item_height );

protected:
	virtual void ApplyStyle() override;

	bool OnKeyDown( const event::event_data_t* data ) override;
	bool OnKeyUp( const event::event_data_t* data ) override;
	bool OnKeyPress( const event::event_data_t* data ) override;

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
	value_t m_value = {};
	size_t m_value_index = 0;
	std::unordered_map< Button*, value_t > m_button_values = {};

	void SetActiveButton( Button* button );

	const std::vector< attribute_type_t > m_forwarded_style_attributes = {
		A_TEXTURE_BACK,
		A_TEXTURE_LEFT,
		A_TEXTURE_TOP,
		A_TEXTURE_RIGHT,
		A_TEXTURE_BOTTOM,
		A_TEXTURE_LEFTTOP,
		A_TEXTURE_RIGHTTOP,
		A_TEXTURE_RIGHTBOTTOM,
		A_TEXTURE_LEFTBOTTOM,
		A_FONT,
		A_TEXT_COLOR,
		A_TEXT_ALIGN,
		A_BUTTON_CLICK_SOUND,
		A_BUTTON_MOVE_SOUND,
		A_BORDER_SIZE,
	};

	void SelectChoice();
};

typedef ChoiceList< size_t > NumChoiceList;
typedef ChoiceList< std::string > AssocChoiceList;

}
}
