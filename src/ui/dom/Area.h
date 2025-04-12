#pragma once

#include "Drawable.h"

namespace ui {

namespace dom {

class Root;

class Area : public Drawable {
public:
	Area( DOM_ARGS_T );

protected:
	virtual const bool IsEventRelevant( const input::Event& event ) const override;

	virtual void WrapEvent( GSE_CALLABLE, const input::Event& e, gse::value::object_properties_t& event_data ) const override;

};

}
}
