#pragma once

#include <vector>
#include <string>

#include "Container.h"

namespace ui {
namespace dom {

class Button;
class Select;

class ChoiceList : public Container {
public:

	ChoiceList( DOM_ARGS );

protected:
	virtual const bool ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) override;
	virtual void WrapEvent( GSE_CALLABLE, const input::Event& e, gse::value::object_properties_t& obj ) const override;

private:
	struct choice_t {
		std::string value;
		std::string label;
		size_t position;
		Button* element;
	};

	std::unordered_map< std::string, choice_t > m_choices = {};
	std::vector< std::string > m_choices_order = {};

	std::string m_itemclass = "";
	size_t m_itemheight = 0;
	size_t m_itempadding = 0;
	std::string m_value = "";
	bool m_keyboard = false;

	const choice_t* m_selected_choice = nullptr;

	void SetItemClass( GSE_CALLABLE, const std::string& cls );
	void SetItemHeight( GSE_CALLABLE, const size_t px );
	void SetItemPadding( GSE_CALLABLE, const size_t px );
	void Clear( GSE_CALLABLE );
	void SetItems( GSE_CALLABLE, const gse::value::array_elements_t& items );
	void SetValue( GSE_CALLABLE, const std::string& value );
	void SetKeyboard( GSE_CALLABLE, const bool value );

	void Select( GSE_CALLABLE, const choice_t& choice, const bool send_event );

private:
	friend class Select;
	typedef std::function< void( const std::string& value, const std::string& label, const bool send_event ) > f_on_update_t;
	f_on_update_t m_on_update = nullptr;
};

}
}
