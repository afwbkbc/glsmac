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

	virtual void SerializeEvent( const input::Event& event, gse::type::object_properties_t& event_data ) const override;

};

}
}
