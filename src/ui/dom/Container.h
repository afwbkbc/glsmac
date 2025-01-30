#pragma once

#include <unordered_set>

#include "Area.h"
#include "gse/Value.h"

namespace scene::actor {
class Cache;
}

namespace ui {
namespace dom {

#define FACTORY( _name, _class ) Factory( GSE_CALL, _name, [ this ]( GSE_CALLABLE, const properties_t& p ) { return new _class( GSE_CALL, m_ui, this, p ); })

class Container : public Area {
public:
	Container( DOM_ARGS_T, const bool factories_allowed );
	virtual ~Container();

	scene::actor::Cache* const m_cache;

	void UpdateMouseOver( GSE_CALLABLE, Object* child );

	const bool ProcessEvent( GSE_CALLABLE, const input::Event& event ) override;

protected:

	virtual const bool ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) override;

	void WrapSet( const std::string& key, const gse::Value& value, GSE_CALLABLE ) override;

	void Property( GSE_CALLABLE, const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value = VALUE( gse::type::Undefined ), const property_flag_t flags = PF_NONE, const f_on_set_t& f_on_set = nullptr, const f_on_unset_t& f_on_unset = nullptr ) override;
	void ForwardProperty( GSE_CALLABLE, const std::string& name, Object* const target );

	void Embed( Object* object );
	void Factory( GSE_CALLABLE, const std::string& name, const std::function< Object*( GSE_CALLABLE, const properties_t& ) >& f );

	void OnPropertyChange( GSE_CALLABLE, const std::string& key, const gse::Value& value ) const override;
	void OnPropertyRemove( GSE_CALLABLE, const std::string& key ) const override;

private:
	friend class Root;

	bool m_is_processing_children_events = false;

	std::unordered_map< id_t, Object* > m_children = {};
	std::vector< Object* > m_embedded_objects = {};
	std::map< std::string, Object* > m_forwarded_properties = {};

	void InitAndValidate( GSE_CALLABLE ) override;

	std::unordered_set< Object* > m_children_to_delete = {};
	void ProcessPendingDeletes( GSE_CALLABLE );

protected:
	friend class ui::Class;

	void SetPropertyFromClass( GSE_CALLABLE, const std::string& key, const gse::Value& value, const class_modifier_t modifier ) override;
	void UnsetPropertyFromClass( GSE_CALLABLE, const std::string& key ) override;

private:
	std::unordered_set< Object* > m_mouse_over_objects = {};
	bool m_processing_mouse_overs = false;

private:
	friend class Object;
	void DeleteChild( GSE_CALLABLE, Object* obj );

};

}
}
