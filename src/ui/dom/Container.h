#pragma once

#include <unordered_set>
#include <map>
#include <vector>
#include <functional>

#include "Area.h"

#include "types/Vec2.h"

namespace scene::actor {
class Cache;
}

namespace ui {
namespace dom {

#define FACTORY( _name, _class ) Factory( GSE_CALL, _name, [ this ]( GSE_CALLABLE, const properties_t& p ) { return new _class( GSE_CALL, m_ui, m_factory_owner, p ); })

class Container : public Area {
public:
	Container( DOM_ARGS_T, const bool factories_allowed );

	void ForwardFactories( GSE_CALLABLE, Object* child );
	void Embed( Object* object, const bool is_visible = true );

	scene::actor::Cache* const m_cache;

	void UpdateMouseOver( GSE_CALLABLE );

	const bool ProcessEvent( GSE_CALLABLE, const input::Event& event ) override;

	void Destroy( GSE_CALLABLE ) override;

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;

	void WrapSet( const std::string& key, gse::Value* const value, GSE_CALLABLE ) override;

protected:
	friend class Scrollview;
	friend class Listview;
	
	Container* m_factory_owner = this;

	std::function< void( const bool is_from_factory ) > m_on_before_add_child = nullptr;
	std::function< void( Object* const obj, const bool is_from_factory ) > m_on_add_child = nullptr;
	std::function< void( Object* const obj ) > m_on_remove_child = nullptr;

	virtual ~Container();

	void Property( GSE_CALLABLE, const std::string& name, const gse::Value::type_t& type, gse::Value* const default_value = nullptr, const property_flag_t flags = PF_NONE, const f_on_set_t& f_on_set = nullptr, const f_on_unset_t& f_on_unset = nullptr ) override;
	void ForwardProperty( GSE_CALLABLE, const std::string& name, Object* const target );
	void ForwardProperty( GSE_CALLABLE, const std::string& srcname, const std::string& dstname, Object* const target );

	void Factory( GSE_CALLABLE, const std::string& name, const std::function< Object*( GSE_CALLABLE, const properties_t& ) >& f );

	void OnPropertyChange( GSE_CALLABLE, const std::string& key, gse::Value* const value ) override;
	void OnPropertyRemove( GSE_CALLABLE, const std::string& key ) override;

private:
	friend class Root;

	std::map< id_t, Object* > m_children = {};
	std::vector< std::pair< Object*, bool > > m_embedded_objects = {};
	std::vector< Object* > m_lazy_embedded_objects = {};
	std::map< std::string, std::vector< std::pair< Object*, std::string > > > m_forwarded_properties = {};

	void InitAndValidate( GSE_CALLABLE ) override;

protected:
	friend class ui::Class;

	void SetPropertyFromClass( GSE_CALLABLE, const std::string& key, gse::Value* const value, const class_modifier_t modifier ) override;
	void UnsetPropertyFromClass( GSE_CALLABLE, const std::string& key ) override;

private:
	Object* m_mouse_over_object = nullptr;
	bool m_processing_mouse_overs = false;

	void SetMouseOverChild( GSE_CALLABLE, Object* obj, const types::Vec2< ssize_t >& mouse_coords );

protected:
	friend class Object;
	friend class Listview;
	void AddChild( GSE_CALLABLE, Object* obj, const bool is_visible );
	void RemoveChild( GSE_CALLABLE, Object* obj, const bool nodestroy = false );

};

}
}
