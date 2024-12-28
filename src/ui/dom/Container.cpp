#include "Container.h"

#include "ui/geometry/Geometry.h"
#include "ui/UI.h"

#include "Surface.h"
#include "Panel.h"
#include "Text.h"
#include "scene/actor/Cache.h"

namespace ui {
namespace dom {

Container::Container( DOM_ARGS_T )
	: Area( DOM_ARGS_PASS_T )
	, m_cache( new scene::actor::Cache( "UI::Cache" ) ) {

	Actor( m_cache );

	m_geometry->m_on_effective_area_update = [ this ]( const area_t& area ) {
		m_cache->SetEffectiveArea( area );
	};
	m_cache->SetEffectiveArea( m_geometry->GetEffectiveArea() );

	FACTORY( "surface", Surface );
	FACTORY( "panel", Panel );
	FACTORY( "text", Text );

}

Container::~Container() {
	for ( const auto& it : m_children ) {
		delete it.second;
	}
}

void Container::WrapSet( const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& si ) {
	auto forward_it = m_forwarded_properties.find( key );
	if ( forward_it != m_forwarded_properties.end() ) {
		forward_it->second->WrapSet( key, value, ctx, si );
	}
	else {
		Object::WrapSet( key, value, ctx, si );
	}
}

void Container::Property( GSE_CALLABLE, const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value, const property_flag_t flags, const f_on_set_t& f_on_set ) {
	ASSERT_NOLOG( m_forwarded_properties.find( name ) == m_forwarded_properties.end(), "property '" + name + "' already exists (forwarded)" );
	Object::Property( ctx, call_si, name, type, default_value, flags, f_on_set );
}

void Container::ForwardProperty( GSE_CALLABLE, const std::string& name, Object* const target ) {
	ASSERT_NOLOG( m_forwarded_properties.find( name ) == m_forwarded_properties.end(), "property '" + name + "' already forwarded" );
	ASSERT_NOLOG( m_properties.find( name ) == m_properties.end(), "property '" + name + "' already taken (exists)" );
	ASSERT_NOLOG( target->m_parent == this, "can only forward properties to direct children" );
	m_forwarded_properties.insert(
		{
			name,
			target
		}
	);
	const auto& it = m_initial_properties.find( name );
	if ( it != m_initial_properties.end() ) {
		target->WrapSet( it->first, it->second, ctx, call_si );
	}
}

void Container::Factory( GSE_CALLABLE, const std::string& name, const std::function< Object*( GSE_CALLABLE, const properties_t& ) >& f ) {
	Method( ctx, call_si, name, NATIVE_CALL( this, f ) {
		N_EXPECT_ARGS_MIN_MAX(0, 1);
		properties_t initial_properties = {};
		if ( arguments.size() == 1 ) {
			const auto& v = arguments.at(0).Get();
			if ( v->type != gse::type::Type::T_OBJECT ) {
				throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Expected properties object, got " + v->GetTypeString( v->type ), ctx, call_si );
			}
			initial_properties = ((gse::type::Object*)v)->value;
		}
		auto* obj = f( ctx, call_si, initial_properties );
		obj->Validate( ctx, call_si );
		m_children.insert({ obj->m_id, obj });
		return obj->Wrap( true );
	} ) );
}

void Container::Validate( GSE_CALLABLE ) const {
	for ( const auto& p : m_initial_properties ) {
		if ( m_properties.find( p.first ) == m_properties.end() && m_forwarded_properties.find( p.first ) == m_forwarded_properties.end() ) {
			throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property '" + p.first + "' does not exist", ctx, call_si );
		}
	}
}

}
}
