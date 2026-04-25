#pragma once

#include <vector>
#include <string>

#include "Container.h"

namespace ui {
namespace dom {

class Button;
class ChoiceList;

class Select : public Container {
public:

	Select( DOM_ARGS );

protected:
	virtual const bool ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) override;
	virtual void WrapEvent( GSE_CALLABLE, const input::Event& e, gse::value::object_properties_t& obj ) const override;

private:
	Button* m_active_element = nullptr;
	ChoiceList* m_choicelist = nullptr;

	bool m_readonly = false;

	void SetReadOnly( GSE_CALLABLE, const bool readonly );
};

}
}
