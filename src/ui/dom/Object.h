#pragma once

#include <map>
#include <unordered_set>

#include "gse/Wrappable.h"
#include "gse/type/Object.h"

#include "ui/Types.h"
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

class Class;

namespace dom {

typedef uint64_t id_t;

class Container;

class Object : public gse::Wrappable {
public:

	Object( DOM_ARGS_T );
	virtual ~Object();

	virtual const gse::Value Wrap( const bool dynamic = false ) override;
	virtual void WrapSet( const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& call_si );
	static void WrapSetStatic( gse::Wrappable* wrapobj, const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& call_si );

	const std::string m_tag;
	Container* const m_parent;
	const id_t m_id;

	virtual geometry::Geometry* const GetGeometry() const;

protected:

	UI* const m_ui;
	const properties_t m_initial_properties;

	enum property_flag_t : uint8_t {
		PF_NONE = 0,
		PF_READONLY = 1 << 0,
	};
	typedef std::function< void( GSE_CALLABLE, const gse::Value& ) > f_on_set_t;
	typedef std::function< void( GSE_CALLABLE ) > f_on_unset_t;

	void Actor( scene::actor::Actor* actor );

	virtual void Property( GSE_CALLABLE, const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value = VALUE( gse::type::Undefined ), const property_flag_t flags = PF_NONE, const f_on_set_t& f_on_set = nullptr, const f_on_unset_t& f_on_unset = nullptr );
	void Method( GSE_CALLABLE, const std::string& name, const gse::Value& callable );

	void ParseColor( GSE_CALLABLE, const std::string& str, types::Color& color ) const;

	virtual void OnPropertyChange( GSE_CALLABLE, const std::string& key, const gse::Value& value ) const;
	virtual void OnPropertyRemove( GSE_CALLABLE, const std::string& key ) const;

private:

	bool m_is_initialized = false;

	struct property_def_t {
		gse::type::Type::type_t type;
		property_flag_t flags;
		f_on_set_t f_on_set;
		f_on_unset_t f_on_unset;
	};
	typedef std::map< std::string, property_def_t > property_defs_t;
	property_defs_t m_property_defs = {};

	properties_t m_properties = {};
	properties_t m_manual_properties = {};
	properties_t m_default_properties = {};
	ui::Class* m_class = nullptr;

	std::vector< scene::actor::Actor* > m_actors = {};

	void SetProperty( GSE_CALLABLE, properties_t* const properties, const std::string& key, const gse::Value& value );
	void UnsetProperty( GSE_CALLABLE, properties_t* const properties, const std::string& key );

	void UnsetClass( GSE_CALLABLE );
	void SetClass( GSE_CALLABLE, const std::string& name );

private:
	friend class Container;

	virtual void InitAndValidate( GSE_CALLABLE );
	void InitProperties( GSE_CALLABLE );

protected:
	friend class ui::Class;
	virtual void SetPropertyFromClass( GSE_CALLABLE, const std::string& key, const gse::Value& value );
	virtual void UnsetPropertyFromClass( GSE_CALLABLE, const std::string& key );

};

}
}
