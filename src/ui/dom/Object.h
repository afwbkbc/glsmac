#pragma once

#include <map>
#include <unordered_set>

#include "gse/Wrappable.h"
#include "gse/type/Object.h"

#define DOM_ARGS GSE_CALLABLE, UI* const ui, Object* const parent, const properties_t& properties
#define DOM_ARGS_T DOM_ARGS, const std::string& tag
#define DOM_ARGS_PASS ctx, call_si, ui, parent, properties
#define DOM_ARGS_PASS_T DOM_ARGS_PASS, tag

namespace ui {

class UI;
class Geometry;

namespace dom {

typedef uint64_t id_t;

class Object : public gse::Wrappable {
public:

	typedef std::map< std::string, gse::Value > properties_t;

	Object( DOM_ARGS_T );

	virtual const gse::Value Wrap( const bool dynamic = false ) override;
	static void WrapSet( gse::Wrappable* wrapobj, const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& si );

	const std::string m_tag;
	Object* const m_parent;
	const id_t m_id;

	virtual Geometry* const GetGeometry() const;

protected:

	UI* const m_ui;
	const properties_t& m_initial_properties;

	enum property_flag_t : uint8_t {
		PF_NONE = 0,
		PF_READONLY = 1 << 0,
	};
	typedef std::function< void( GSE_CALLABLE, const gse::Value& ) > f_on_set_t;

	void Property( GSE_CALLABLE, const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value = VALUE( gse::type::Undefined ), const property_flag_t flags = PF_NONE, const f_on_set_t& f_on_set = nullptr );
	void Method( GSE_CALLABLE, const std::string& name, const gse::Value& callable );

private:

	struct property_t {
		gse::type::Type::type_t type;
		gse::Value value;
		property_flag_t flags;
		f_on_set_t f_on_set;
	};
	std::map< std::string, property_t > m_properties = {};
	std::unordered_set< std::string > m_changes = {};

private:
	friend class Container;
	void Validate( GSE_CALLABLE ) const;

};

}
}
