#pragma once

#include "Panel.h"

#include "util/Timer.h"

namespace ui {
namespace dom {

class Text;

class Input : public Panel {
public:
	Input( DOM_ARGS );

protected:
	virtual const bool ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) override;

	virtual void SerializeEvent( const input::Event& e, gse::value::object_properties_t& obj ) const override;

private:
	std::string m_value = "";

	Text* m_text;

	bool m_is_blink_cursor_visible = false;
	util::Timer m_blinker;

	void SetValue( GSE_CALLABLE, const std::string& value );
	void FixAlign();

};

}
}
