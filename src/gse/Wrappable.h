#pragma once

#include <unordered_set>
#include <unordered_map>
#include <map>

#include "type/Types.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Undefined.h"
#include "callable/Native.h"
#include "Exception.h"
#include "Value.h"

namespace gse {

namespace type {
class Object;
}

class Wrappable {
public:
	virtual ~Wrappable();

	virtual const gse::Value Wrap( const bool dynamic = false ) = 0;

	void Link( type::Object* wrapobj );
	void Unlink( type::Object* wrapobj );

	typedef uint16_t callback_id_t;
	const callback_id_t On( GSE_CALLABLE, const std::string& event, const gse::Value& callback );
	void Off( GSE_CALLABLE, const std::string& event, const callback_id_t callback_id );
	const Value Trigger( GSE_CALLABLE, const std::string& event, const type::object_properties_t& args );

private:
	std::unordered_set< type::Object* > m_wrapobjs = {};

	typedef std::unordered_map< std::string, std::map< uint16_t, gse::Value > > callbacks_t;
	callbacks_t m_callbacks = {};
	callback_id_t m_next_callback_id = 0;
};

}
