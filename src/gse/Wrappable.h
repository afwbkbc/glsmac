#pragma once

#include <unordered_set>
#include <unordered_map>
#include <map>
#include <optional>
#include <functional>

#include "value/Types.h"
#include "value/Int.h"
#include "value/String.h"
#include "value/Undefined.h"
#include "value/Object.h"
#include "callable/Native.h"
#include "Exception.h"
#include "Value.h"

namespace gse {

namespace value {
class Object;
}

class Wrappable {
public:
	virtual ~Wrappable();

	virtual Value* const Wrap( GSE_CALLABLE, const bool dynamic = false ) = 0;

	void Link( value::Object* wrapobj );
	void Unlink( value::Object* wrapobj );

	typedef uint16_t callback_id_t;
	typedef std::function< void( value::object_properties_t& args ) > f_args_t;
	virtual const callback_id_t On( GSE_CALLABLE, const std::string& event, Value* const callback );
	virtual void Off( GSE_CALLABLE, const std::string& event, const callback_id_t callback_id );
	virtual const bool HasHandlers( const std::string& event );
	virtual Value* const Trigger( GSE_CALLABLE, const std::string& event, const f_args_t& f_args = nullptr, const std::optional< Value::type_t > expected_return_type = {} );

protected:
	// TODO: wrapobjs mutex
	std::unordered_set< value::Object* > m_wrapobjs = {};

protected:
	struct callback_t {
		Value* const callable;
		context::Context* ctx;
		si_t si;
	};
	typedef std::unordered_map< std::string, std::map< uint16_t, callback_t > > callbacks_t;
	callbacks_t m_callbacks = {};
	callback_id_t m_next_callback_id = 0;
};

}
