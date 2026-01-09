#pragma once

#include <map>
#include <unordered_set>
#include <mutex>

#include "env/Env.h"

#include "gse/GCWrappable.h"
#include "gc/Object.h"
#include "gse/value/Object.h"

#include "ui/Types.h"
#include "types/Color.h"
#include "input/Types.h"

#define DOM_ARGS GSE_CALLABLE, UI* const ui, Container* const parent, const properties_t& properties
#define DOM_ARGS_T DOM_ARGS, const std::string& tag
#define DOM_ARGS_TD( _tag ) DOM_ARGS, const std::string& tag = _tag
#define DOM_ARGS_PASS GSE_CALL, ui, parent, properties
#define DOM_ARGS_PASS_T DOM_ARGS_PASS, tag

#define PROPERTY( _name, _type, _default, _setter ) \
    Property( \
        GSE_CALL, _name, _type::GetType(), VALUE( _type, , _default ), PF_NONE, \
        [ this ]( GSE_CALLABLE, gse::Value* const v ) { \
            _setter( GSE_CALL, ( (_type*)v )->value ); \
        }, \
        [ this ]( GSE_CALLABLE ) { \
            _setter( GSE_CALL, _default ); \
        } \
    );

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

class Container;

class Object : public gse::GCWrappable {
public:

	Object( DOM_ARGS_T );

	virtual gse::Value* const Wrap( GSE_CALLABLE, const bool dynamic = false ) override;
	virtual void WrapSet( const std::string& key, gse::Value* const value, GSE_CALLABLE );
	static void WrapSetStatic( gse::Wrappable* wrapobj, const std::string& key, gse::Value* const value, GSE_CALLABLE );

	const std::string m_tag;
	Container* m_parent;
	const object_id_t m_id;

	virtual geometry::Geometry* const GetGeometry() const;

	virtual const bool ProcessEvent( GSE_CALLABLE, const input::Event& event );

	virtual void Destroy( GSE_CALLABLE );

	virtual void Show();
	virtual void Hide();
	void Refresh();

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;

	void AddModifier( GSE_CALLABLE, const class_modifier_t modifier );
	void RemoveModifier( GSE_CALLABLE, const class_modifier_t modifier );

	void Globalize( GSE_CALLABLE, const std::function< void() >& f_on_deglobalize = nullptr );
	void Deglobalize( GSE_CALLABLE );

	bool m_is_hoverable = true;

	static Object* Unwrap( gse::Value* const value );
	static const gse::value::Object::object_class_t WRAP_CLASS;

protected:

	virtual ~Object();

	virtual const bool IsEventRelevant( const input::Event& event ) const;
	virtual const bool ProcessEventImpl( GSE_CALLABLE, const input::Event& event );

	void UpdateProperty( const std::string& name, gse::Value* const v );

	UI* const m_ui;
	gc::Space* const m_gc_space;
	const properties_t m_initial_properties;

	enum property_flag_t : uint8_t {
		PF_NONE = 0,
		PF_READONLY = 1 << 0,
	};
	typedef std::function< void( GSE_CALLABLE, gse::Value* const ) > f_on_set_t;
	typedef std::function< void( GSE_CALLABLE ) > f_on_unset_t;

	void Actor( scene::actor::Actor* actor );
	void ClearActors();

	virtual void Property( GSE_CALLABLE, const std::string& name, const gse::Value::type_t& type, gse::Value* const default_value = nullptr, const property_flag_t flags = PF_NONE, const f_on_set_t& f_on_set = nullptr, const f_on_unset_t& f_on_unset = nullptr );
	void Method( GSE_CALLABLE, const std::string& name, gse::Value* const callable );
	void Events( const std::unordered_set< input::event_type_t >& events );
	void Iterable( const std::function< void() >& f );

	const bool TryParseColor( GSE_CALLABLE, const std::string& str, types::Color& color ) const;
	void ParseColor( GSE_CALLABLE, const std::string& str, types::Color& color ) const;

	virtual void OnPropertyChange( GSE_CALLABLE, const std::string& key, gse::Value* const value );
	virtual void OnPropertyRemove( GSE_CALLABLE, const std::string& key );

	virtual void WrapEvent( GSE_CALLABLE, const input::Event& e, gse::value::object_properties_t& event_data ) const;

	bool m_is_visible = false;

private:

	std::unordered_set< input::event_type_t > m_supported_events = {};

	bool m_is_initialized = false;
	bool m_is_destroyed = false;

	struct property_def_t {
		gse::Value::type_t type;
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
	std::mutex m_actors_mutex;

	void SetProperty( GSE_CALLABLE, properties_t* const properties, const std::string& key, gse::Value* const value );
	void UnsetProperty( GSE_CALLABLE, properties_t* const properties, const std::string& key );

	void SetClasses( GSE_CALLABLE, const std::vector< std::string >& names );

	class_modifiers_t m_modifiers = {};

	gse::value::Object* m_wrapobj = nullptr;

	bool m_is_iterable_set = false;
	bool m_is_globalized = false;

private:
	friend class Container;

	virtual void InitAndValidate( GSE_CALLABLE );
	void InitProperties( GSE_CALLABLE );

	// in case parent is gc-ed before children
	void Detach();

protected:
	friend class ui::Class;

	virtual void SetPropertyFromClass( GSE_CALLABLE, const std::string& key, gse::Value* const value, const class_modifier_t modifier );
	virtual void UnsetPropertyFromClass( GSE_CALLABLE, const std::string& key );

};

}
}
