#pragma once

#include "Panel.h"

namespace ui {
namespace dom {

class Text;

class Button : public Panel {
public:
	Button( DOM_ARGS );

protected:
	virtual const bool ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) override;

	virtual void SerializeEvent( const input::Event& e, gse::value::object_properties_t& obj ) const override;

private:
	Text* m_label;
	input::mouse_button_t m_last_button = input::MB_NONE;

	bool m_is_ok = false;
	bool m_is_cancel = false;

};

}
}
