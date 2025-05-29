#pragma once

#include "Panel.h"
#include "Focusable.h"

#include "util/Timer.h"

namespace ui {
namespace dom {

class Text;

class Input : public Panel, public Focusable {
public:
	Input( DOM_ARGS );

	void OnFocus() override;
	void OnDefocus() override;

	void Show() override;
	void Hide() override;

protected:
	virtual const bool ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) override;

	virtual void WrapEvent( GSE_CALLABLE, const input::Event& e, gse::value::object_properties_t& obj ) const override;

private:

	bool m_is_focused = false;

	std::string m_value = "";

	Text* m_text;

	bool m_is_blink_cursor_visible = false;
	util::Timer m_blinker;

	void SetValue( GSE_CALLABLE, const std::string& value );
	void FixAlign();

	void UpdateText();

};

}
}
