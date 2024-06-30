#pragma once

#include "Panel.h"

#include "types/Color.h"

// TODO: remove this
#include "ChoiceList.h"

namespace ui {
namespace object {

class Label;
class SimpleButton;

/* Extendable dropdown with selection */

template< typename KEY_TYPE >
CLASS( Dropdown, Panel )

	enum dropdown_mode_t {
		DM_SELECT, // value must be contained in choices, value is updated on change
		DM_MENU, // value is independent from choices and not updated
	};

	Dropdown( const std::string& class_name );

	void SetChoices( const typename ChoiceList< KEY_TYPE >::choices_t& choices );
	void SetValueByKey( const typename ChoiceList< KEY_TYPE >::value_t value );
	void SetValue( const std::string& value );
	void SetMode( const dropdown_mode_t mode );

	// TODO: refactor and remove
	void SetChoicesV( const std::vector< std::string >& labels );

	void SetTextColor( const types::Color& color );

	void Create() override;
	void Destroy() override;
	void Align() override;

private:

	dropdown_mode_t m_mode = DM_SELECT;

	bool m_custom_text_color = false;
	types::Color m_text_color = {};

	const bool IsExpanded() const;
	void Expand();
	void Collapse();

	struct {
		Label* value = nullptr;
		SimpleButton* open_close = nullptr;
		ChoiceList< KEY_TYPE >* choices = nullptr;
	} m_elements;

	typename ChoiceList< KEY_TYPE >::choices_t m_choices = {};
	std::string m_value = "";

};

typedef Dropdown< size_t > NumDropdown;
typedef Dropdown< std::string > AssocDropdown;

}
}
