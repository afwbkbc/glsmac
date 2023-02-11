#pragma once

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
	
	void SetMaxLength( const size_t max_length );
	
	void Focus();
	void Defocus();

protected:
	
	bool OnKeyDown( const UIEvent::event_data_t* data );
	
	std::string m_value = "";
	size_t m_max_length = 0;
	
	Label* m_label = nullptr;
	
	std::string m_cursor_blink_value = " ";
	util::Timer m_cursor_blink_timer;
	
	void UpdateValueLabel();
	
};

} /* namespace object */
} /* namespace ui */
