#pragma once

#include "Area.h"

namespace scene::actor {
class Cache;
}

namespace ui {
namespace dom {

#define FACTORY( _name, _class ) Factory( ctx, call_si, _name, [ this ]( GSE_CALLABLE, const properties_t& p ) { return new _class( ctx, call_si, m_ui, this, p ); })

class Container : public Area {
public:
	Container( DOM_ARGS_T );
	virtual ~Container();

	scene::actor::Cache* const m_cache;

protected:

	void WrapSet( const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& call_si ) override;

	void Property( GSE_CALLABLE, const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value = VALUE( gse::type::Undefined ), const property_flag_t flags = PF_NONE, const f_on_set_t& f_on_set = nullptr, const f_on_unset_t& f_on_unset = nullptr ) override;
	void ForwardProperty( GSE_CALLABLE, const std::string& name, Object* const target );

	void Embed( Object* object );
	void Factory( GSE_CALLABLE, const std::string& name, const std::function< Object*( GSE_CALLABLE, const properties_t& ) >& f );

	void OnPropertyChange( GSE_CALLABLE, const std::string& key, const gse::Value& value ) const override;
	void OnPropertyRemove( GSE_CALLABLE, const std::string& key ) const override;

private:
	friend class Root;

	std::unordered_map< id_t, Object* > m_children = {};
	std::vector< Object* > m_embedded_objects = {};
	std::map< std::string, Object* > m_forwarded_properties = {};

	void InitAndValidate( GSE_CALLABLE ) override;

protected:
	friend class ui::Class;
	void SetPropertyFromClass( GSE_CALLABLE, const std::string& key, const gse::Value& value ) override;
	void UnsetPropertyFromClass( GSE_CALLABLE, const std::string& key ) override;

};

}
}
