#pragma once

/* Clickable button */

#include "Panel.h"
#include "Label.h"

#include "util/Timer.h"

namespace ui {
namespace object {

CLASS( Input, Panel )

	Input( const string& class_name = "" );

	virtual void Create();
	virtual void Iterate();
	virtual void Destroy();
	
	void SetValue( const string& value );
	const string& GetValue() const;
	
	void SetMaxLength( const size_t max_length );
	
	void Focus();
	void Defocus();

protected:
	
	bool OnKeyDown( const UIEvent::event_data_t* data );
	
	string m_value = "";
	size_t m_max_length = 0;
	
	Label* m_label = nullptr;
	
	string m_cursor_blink_value = " ";
	util::Timer m_cursor_blink_timer;
	
	void UpdateValueLabel();
	
};

} /* namespace object */
} /* namespace ui */
