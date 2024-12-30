#pragma once

#include <map>
#include <unordered_set>

#include "gse/Wrappable.h"
#include "gse/type/Object.h"

#include "types/Color.h"

#define DOM_ARGS GSE_CALLABLE, UI* const ui, Container* const parent, const properties_t& properties
#define DOM_ARGS_T DOM_ARGS, const std::string& tag
#define DOM_ARGS_PASS ctx, call_si, ui, parent, properties
#define DOM_ARGS_PASS_T DOM_ARGS_PASS, tag

namespace scene {
namespace actor {
class Actor;
}
}

namespace ui {

class UI;

namespace geometry {
class Geometry;
}

namespace dom {

typedef uint64_t id_t;

class Container;

class Object : public gse::Wrappable {
public:

	typedef std::map< std::string, gse::Value > properties_t;

	Object( DOM_ARGS_T );
	virtual ~Object();

	virtual const gse::Value Wrap( const bool dynamic = false ) override;
	virtual void WrapSet( const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& si );
	static void WrapSetStatic( gse::Wrappable* wrapobj, const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& si );

	const std::string m_tag;
	Container* const m_parent;
	const id_t m_id;

	virtual geometry::Geometry* const GetGeometry() const;

protected:

	UI* const m_ui;
	const properties_t& m_initial_properties;

	enum property_flag_t : uint8_t {
		PF_NONE = 0,
		PF_READONLY = 1 << 0,
	};
	typedef std::function< void( GSE_CALLABLE, const gse::Value& ) > f_on_set_t;

	void Actor( scene::actor::Actor* actor );

	virtual void Property( GSE_CALLABLE, const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value = VALUE( gse::type::Undefined ), const property_flag_t flags = PF_NONE, const f_on_set_t& f_on_set = nullptr );
	void Method( GSE_CALLABLE, const std::string& name, const gse::Value& callable );

	void ParseColor( GSE_CALLABLE, const std::string& str, types::Color& color ) const;

private:

	struct property_t {
		gse::type::Type::type_t type;
		gse::Value value;
		property_flag_t flags;
		f_on_set_t f_on_set;
	};
	std::map< std::string, property_t > m_properties = {};
	std::unordered_set< std::string > m_changes = {};

	std::vector< scene::actor::Actor* > m_actors = {};

private:
	friend class Container;

	virtual void InitAndValidate( GSE_CALLABLE ) const;
	void InitProperties( GSE_CALLABLE ) const;

};

}
}
