#pragma once

#include <map>
#include <unordered_set>

#include "gse/Wrappable.h"
#include "gse/type/Object.h"

#include "ui/Types.h"
#include "types/Color.h"
#include "input/Types.h"

#define DOM_ARGS GSE_CALLABLE, UI* const ui, Container* const parent, const properties_t& properties
#define DOM_ARGS_T DOM_ARGS, const std::string& tag
#define DOM_ARGS_TD( _tag ) DOM_ARGS, const std::string& tag = _tag
#define DOM_ARGS_PASS GSE_CALL, ui, parent, properties
#define DOM_ARGS_PASS_T DOM_ARGS_PASS, tag

namespace scene {
namespace actor {
class Actor;
}
}

namespace input {
class Event;
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

	virtual const bool ProcessEvent( GSE_CALLABLE, const input::Event& event );

protected:

	virtual const bool IsEventRelevant( const input::Event& event ) const;
	virtual const bool ProcessEventImpl( GSE_CALLABLE, const input::Event& event );

	void UpdateProperty( const std::string& name, const gse::Value& v );

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
	void Events( const std::unordered_set< input::event_type_t >& events );
	void Iterable( const std::function< void() >& f );

	const bool TryParseColor( GSE_CALLABLE, const std::string& str, types::Color& color ) const;
	void ParseColor( GSE_CALLABLE, const std::string& str, types::Color& color ) const;

	virtual void OnPropertyChange( GSE_CALLABLE, const std::string& key, const gse::Value& value ) const;
	virtual void OnPropertyRemove( GSE_CALLABLE, const std::string& key ) const;

	virtual void SerializeEvent( const input::Event& event, gse::type::object_properties_t& event_data ) const;

	void AddModifier( GSE_CALLABLE, const class_modifier_t& modifier );
	void RemoveModifier( GSE_CALLABLE, const class_modifier_t& modifier );
private:

	std::unordered_set< input::event_type_t > m_supported_events = {};

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
	std::vector< ui::Class* > m_classes = {};

	std::vector< scene::actor::Actor* > m_actors = {};

	void SetProperty( GSE_CALLABLE, properties_t* const properties, const std::string& key, const gse::Value& value );
	void UnsetProperty( GSE_CALLABLE, properties_t* const properties, const std::string& key );

	void SetClasses( GSE_CALLABLE, const std::vector< std::string >& names );

	class_modifiers_t m_modifiers = {};

	std::shared_ptr< gse::type::Object > m_wrapobj = nullptr;

	bool m_is_iterable_set = false;

private:
	friend class Container;

	virtual void InitAndValidate( GSE_CALLABLE );
	void InitProperties( GSE_CALLABLE );

protected:
	friend class ui::Class;

	virtual void SetPropertyFromClass( GSE_CALLABLE, const std::string& key, const gse::Value& value, const class_modifier_t modifier );
	virtual void UnsetPropertyFromClass( GSE_CALLABLE, const std::string& key );

};

}
}
