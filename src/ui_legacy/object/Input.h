#pragma once

#include <unordered_map>

#include "Panel.h"

/* Text input */

#include "ui_legacy/theme/Types.h"

#include "util/Timer.h"

namespace ui_legacy {
namespace object {

class Label;

CLASS( Input, Panel )

	Input( const std::string& class_name = "" );

	virtual void Create() override;
	virtual void Iterate() override;
	virtual void Destroy() override;

	void SetValue( const std::string& value );
	const std::string& GetValue() const;
	void SetMaxLength( const size_t max_length );

	void SetHint( const std::string& hint );
	const std::string& GetHint() const;

	void Clear();

	void Focus() override;
	void Defocus() override;

private:

	struct {
		const std::unordered_map< attribute_type_t, attribute_type_t > value_label = {
			{ A_FONT,       A_FONT },
			{ A_TEXT_COLOR, A_TEXT_COLOR },
		};
		const std::unordered_map< attribute_type_t, attribute_type_t > hint_label = {
			{ A_FONT,       A_FONT },
			{ A_HINT_COLOR, A_TEXT_COLOR },
		};
	} m_forwarded_style_attributes;

	bool OnKeyDown( const event::event_data_t* data ) override;

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

}
}
