#pragma once

#include <string>

#include "Module.h"

#include "ui/object/Section.h"
#include "ui/object/Button.h"

#include "util/Timer.h"

namespace ui {
namespace module {

CLASS( Error, Module )
	
	Error( UI* ui );
	~Error();
	
	void Show( const std::string& text, const ui_handler_t on_close = UH() {} );
	void Hide();
	
	void SetText( const std::string& error_text );
	
	void ProcessEvent( event::UIEvent* event );
	
private:
	
	void Start();
	void Stop();

	void Close();
	
	std::string m_error_text = "";
	
	Section* m_section = nullptr;
	Label* m_label = nullptr;
	Button* m_button_close = nullptr;
	
	ui_handler_t m_on_close = 0;
	
};
	
}
}
