#pragma once

#include <string>

#include "Module.h"

#include "ui/Types.h"

namespace ui {

namespace object {
class Section;
class Label;
class Button;
}

namespace module {

CLASS( Error, Module )

	Error( UI* ui );
	~Error();

	void Show( const std::string& text, const ui_handler_t on_close = UH() {} );
	void Hide();
	const bool IsActive() const;

	void SetText( const std::string& error_text );

	void ProcessEvent( event::UIEvent* event ) override;

private:

	void Start();
	void Stop();

	void Close();

	std::string m_error_text = "";

	object::Section* m_section = nullptr;
	object::Label* m_label = nullptr;
	object::Button* m_button_close = nullptr;

	ui_handler_t m_on_close = 0;

};

}
}
