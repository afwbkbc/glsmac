#pragma once

#include "base/Base.h"

#include "Label.h"
#include "SimpleButton.h"
#include "ChoiceList.h"

namespace ui {
namespace object {

/* Extendable dropdown with selection */
	
CLASS( Dropdown, Panel )
	
	enum dropdown_mode_t {
		DM_SELECT, // value must be contained in choices, value is updated on change
		DM_MENU, // value is independent from choices and not updated
	};
	
	Dropdown( const std::string& class_name );

	void SetChoices( const ChoiceList::choices_t& choices );
	void SetValue( const std::string& value );
	void SetMode( const dropdown_mode_t mode );
	
	// TODO: refactor and remove
	void SetChoicesV( const std::vector< std::string >& labels );
	
	void Create();
	void Destroy();
	void Align();
	
private:
	
	dropdown_mode_t m_mode = DM_SELECT;
	
	const bool IsExpanded() const;
	void Expand();
	void Collapse();
	
	struct {
		Label* value = nullptr;
		SimpleButton* open_close = nullptr;
		ChoiceList *choices = nullptr;
	} m_elements;
	
	ChoiceList::choices_t m_choices = {};
	std::string m_value = "";

};

}
}
