#pragma once

#include "Panel.h"

namespace ui {
namespace dom {

class Text;
class Sound;

class Button : public Panel {
public:
	Button( DOM_ARGS );

	typedef std::function< void() > f_on_click_t;
	f_on_click_t m_on_mousedown = nullptr;
	f_on_click_t m_on_mouseup = nullptr;
	f_on_click_t m_on_click = nullptr;

protected:
	virtual const bool ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) override;
	virtual void WrapEvent( GSE_CALLABLE, const input::Event& e, gse::value::object_properties_t& obj ) const override;

private:
	Text* m_label;
	Sound* m_sound;

	input::mouse_button_t m_last_button = input::MB_NONE;

	bool m_is_ok = false;
	bool m_is_cancel = false;

};

}
}
