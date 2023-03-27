#pragma once

#include <vector>
#include <unordered_map>

/* Clickable button */

#include "Panel.h"
#include "Label.h"

#include "util/Timer.h"

namespace ui {
namespace object {

CLASS( Input, Panel )

	Input( const std::string& class_name = "" );

	virtual void Create();
	virtual void Iterate();
	virtual void Destroy();
	
	void SetValue( const std::string& value );
	const std::string& GetValue() const;
	void SetHint( const std::string& hint );
	void SetMaxLength( const size_t max_length );
	
	void Clear();
	
	void Focus();
	void Defocus();

private:
	
	struct {
		const std::unordered_map< Style::attribute_type_t, Style::attribute_type_t > value_label = {
			{ Style::A_FONT, Style::A_FONT },
			{ Style::A_TEXT_COLOR, Style::A_TEXT_COLOR },
		};
		const std::unordered_map< Style::attribute_type_t, Style::attribute_type_t > hint_label = {
			{ Style::A_FONT, Style::A_FONT },
			{ Style::A_HINT_COLOR, Style::A_TEXT_COLOR },
		};
	} m_forwarded_style_attributes;
	
	bool OnKeyDown( const UIEvent::event_data_t* data );
	
	std::string m_value = "";
	size_t m_max_length = 0;
	
	std::string m_hint = "";
	
	Label* m_value_label = nullptr;
	Label* m_hint_label = nullptr;
	
	std::string m_cursor_blink_value = " ";
	util::Timer m_cursor_blink_timer;
	
	void UpdateValueLabel( const bool send_event = false );
	void UpdateHintLabel();
	
};

} /* namespace object */
} /* namespace ui */
