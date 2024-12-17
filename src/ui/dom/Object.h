#pragma once

#include <map>
#include <unordered_set>

#include "gse/Wrappable.h"
#include "gse/type/Object.h"

namespace ui {
namespace dom {

typedef uint64_t id_t;

class Object : public gse::Wrappable {
public:

	typedef std::map< std::string, gse::Value > properties_t;

	Object( const std::string& cls, Object* const parent, const properties_t& properties );

	virtual const gse::Value Wrap( const bool dynamic = false ) override;
	static void WrapSet( gse::Wrappable* wrapobj, const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& si );
	void OnWrapSet( const std::string& property_name );

	const std::string m_class;
	Object* const m_parent;
	const id_t m_id;

protected:

	const properties_t& m_initial_properties;

	enum property_flag_t : uint8_t {
		PF_NONE = 0,
		PF_READONLY = 1 << 0,
	};
	void Property( const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value = VALUE( gse::type::Undefined ), const property_flag_t flags = PF_NONE );
	void Method( const std::string& name, const gse::Value& callable );

private:

	struct property_t {
		gse::type::Type::type_t type;
		gse::Value value;
		property_flag_t flags;
	};
	std::map< std::string, property_t > m_properties = {};
	std::unordered_set< std::string > m_changes = {};
};

}
}
