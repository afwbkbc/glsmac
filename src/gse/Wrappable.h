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

#include "util/Struct.h"

namespace gse {

namespace value {
class Object;
}

class Wrappable {
public:

	template< typename T >
	class wrapmap_t {
	public:
		typedef const std::unordered_map< T, std::string > t_type_to_string;
		typedef const std::unordered_map< std::string, T > t_string_to_type;
		wrapmap_t()
			: m_type_to_string( {} )
			, m_string_to_type( {} ) {}
		wrapmap_t( const t_type_to_string& type_to_string )
			: m_type_to_string( type_to_string )
			, m_string_to_type( util::Struct::FlipMap( type_to_string ) ) {}
		const t_type_to_string& GetVK() const {
			return m_type_to_string;
		}
		const t_string_to_type& GetKV() const {
			return m_string_to_type;
		}
		const T& GetValue( GSE_CALLABLE, const std::string& str ) const {
			const auto& it = m_string_to_type.find( str );
			if ( it == m_string_to_type.end() ) {
				std::string supported_values = "";
				for ( const auto& v : m_string_to_type ) {
					supported_values += " " + v.first;
				}
				GSE_ERROR( EC.INVALID_DEFINITION, "Unknown value '" + str + "', supported values:" + supported_values );
			}
			return it->second;
		}
		const std::string& GetString( const T& value ) const {
			ASSERT_NOLOG( m_type_to_string.find( value ) != m_type_to_string.end(), "value not in wrapmap" );
			return m_type_to_string.at( value );
		}
		const T& GetValueUnsafe( const std::string& value ) const {
			ASSERT_NOLOG( m_string_to_type.find( value ) != m_string_to_type.end(), "string not in wrapmap" );
			return m_string_to_type.at( value );
		}
		Value* const Get( GSE_CALLABLE, const T& value ) const {
			return VALUE( value::String, , GetString( value ) );
		}
	private:
		const t_type_to_string m_type_to_string;
		const t_string_to_type m_string_to_type;
	};

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
