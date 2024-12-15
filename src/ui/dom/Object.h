#pragma once

#include <map>
#include <unordered_set>

#include "gse/Wrappable.h"
#include "gse/type/Object.h"

namespace ui {
namespace dom {

class Object : public gse::Wrappable {
public:

	Object();

	static const gse::type::Object::object_class_t WRAP_CLASS;
	virtual const gse::Value Wrap( const bool dynamic = false ) override;
	static Object* Unwrap( const gse::Value& value );
	static void WrapSet( gse::Wrappable* wrapobj, const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& si );
	void OnWrapSet( const std::string& property_name );

protected:
	void Property( const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value = VALUE( gse::type::Undefined ) );

private:

	struct property_t {
		gse::type::Type::type_t type;
		gse::Value value;
	};
	std::map< std::string, property_t > m_properties = {};
	std::unordered_set< std::string > m_changes = {};
};

}
}
