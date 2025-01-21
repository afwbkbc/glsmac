#include "Class.h"

#include "ui/UI.h"
#include "ui/dom/Object.h"
#include "gse/type/Array.h"

namespace ui {

static const std::unordered_map< std::string, Class::modifier_t > s_name_to_modifier = {
	{ "_hover", Class::CM_HOVER },
	{ "_active", Class::CM_ACTIVE },
	{ "_focus", Class::CM_FOCUS },
};

static const std::unordered_map< Class::modifier_t, std::string > s_modifier_to_name = {
	{ Class::CM_HOVER, "_hover" },
	{ Class::CM_ACTIVE, "_active" },
	{ Class::CM_FOCUS, "_focus" },
};

Class::Class( const UI* const ui,  const bool is_master )
	: m_ui( ui )
	, m_is_master( is_master ) {
	if ( m_is_master ) {
		m_subclasses = {
			{ CM_HOVER, new Class( ui, false ) },
			{ CM_ACTIVE, new Class( ui, false ) },
			{ CM_FOCUS, new Class( ui, false ) },
		};
	}
}

Class::~Class() {
	if ( m_is_master ) {
		for ( const auto& it : m_subclasses ) {
			delete it.second;
		}
	}
}

const properties_t& Class::GetProperties() const {
	return m_properties;
}

void Class::AddObject( GSE_CALLABLE, dom::Object* object ) {
	ASSERT_NOLOG( m_objects.find( object ) == m_objects.end(), "object already exists in class" );
	m_objects.insert( object );
	UpdateObject( ctx, call_si, object );
}

void Class::RemoveObject( GSE_CALLABLE, dom::Object* object ) {
	ASSERT_NOLOG( m_objects.find( object ) != m_objects.end(), "object not found in class" );
	for ( const auto& property : m_properties ) {
		object->UnsetPropertyFromClass( ctx, call_si, property.first );
	}
	m_objects.erase( object );
}

const gse::Value Class::Wrap( const bool dynamic ) {
	gse::type::object_properties_t properties = {
		{
			"extend",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( name, 0, String );
				SetParentClass( ctx, call_si, name );
				return Wrap( true );
			} )
		},
		{
			"set",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( properties, 0, Object );
				SetProperties( ctx, call_si, properties );
				return Wrap( true );
			} )
		},
		{
			"unset",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( properties, 0, Array );
				std::vector< std::string > names = {};
				names.reserve( properties.size() );
				for ( const auto& v : properties ) {
					if ( v.Get()->type != gse::type::Type::T_STRING ) {
						GSE_ERROR( gse::EC.UI_ERROR, "Expected array of property names, got: " + arguments.at( 0 ).ToString() );
					}
					names.push_back( ((gse::type::String*)v.Get())->value );
				}
				UnsetProperties( ctx, call_si, names );
				return Wrap( true );
			} )
		},
	};
	for ( const auto& p : m_properties ) {
		properties.insert(
			{
				p.first,
				p.second
			}
		);
	}
	if ( m_is_master ) {
		for ( const auto& c : m_subclasses ) {
			properties.insert({ s_modifier_to_name.at( c.first ), c.second->Wrap( dynamic ) });
		}
	}
	return gse::Value(
		std::make_shared< gse::type::Object >(
			nullptr, properties, "class", this, dynamic
				? &Class::WrapSetStatic
				: nullptr
		)
	);
}

void Class::WrapSet( const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& call_si ) {
	SetProperty( ctx, call_si, key, value );
}

void Class::WrapSetStatic( gse::Wrappable* wrapobj, const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& call_si ) {
	ASSERT_NOLOG( wrapobj, "wrapobj not set" );
	( (Class*)wrapobj )->WrapSet( key, value, ctx, call_si );
}

void Class::SetProperty( gse::context::Context* ctx, const gse::si_t& call_si, const std::string& name, const gse::Value& value ) {
	const auto& it_old = m_properties.find( name );
	if ( it_old == m_properties.end() || it_old->second != value ) {
		m_properties.insert_or_assign( name, value );
		for ( const auto& obj : m_wrapobjs ) {
			obj->value.insert_or_assign( name, value );
		}
		for ( const auto& cls : m_child_classes ) {
			cls->SetPropertyFromParent( ctx, call_si, name, value );
		}
		for ( const auto& object : m_objects ) {
			object->SetPropertyFromClass( ctx, call_si, name, value );
		}
	}
}

void Class::SetPropertyFromParent( GSE_CALLABLE, const std::string& name, const gse::Value& value ) {
	const auto& local_it = m_local_properties.find( name );
	if ( local_it == m_local_properties.end() ) { // local properties should have priority
		SetProperty( ctx, call_si, name, value );
	}
}

void Class::SetPropertiesFromParent( GSE_CALLABLE ) {
	ASSERT_NOLOG( m_parent_class, "parent class not set" );
	for ( const auto& it : m_parent_class->m_properties ) {
		SetPropertyFromParent( ctx, call_si, it.first, it.second );
	}
}

void Class::UnsetProperty( GSE_CALLABLE, const std::string& name ) {
	const auto& it = m_properties.find( name );
	if ( it != m_properties.end() ) {
		m_properties.erase( it );
		for ( const auto& cls : m_child_classes ) {
			cls->UnsetPropertyFromParent( ctx, call_si, name );
		}
		for ( const auto& object : m_objects ) {
			object->UnsetPropertyFromClass( ctx, call_si, name );
		}
	}
}

void Class::UnsetPropertyFromParent( GSE_CALLABLE, const std::string& name ) {
	const auto& local_it = m_local_properties.find( name );
	if ( local_it == m_local_properties.end() ) { // otherwise it would have been overridden by local property anyway
		UnsetProperty( ctx, call_si, name );
	}
}

void Class::UnsetProperties( GSE_CALLABLE, const std::vector< std::string >& properties ) {
	for ( const auto& name : properties ) {
		const auto& it = m_local_properties.find( name );
		if ( it != m_local_properties.end() ) { // otherwise this property did not exist anyway
			m_local_properties.erase( it );
			if ( m_parent_class ) {
				const auto& parent_it = m_parent_class->m_properties.find( name );
				if ( parent_it != m_parent_class->m_properties.end() ) {
					// inherit from parent
					SetProperty( ctx, call_si, name, parent_it->second );
					continue;
				}
			}
			UnsetProperty( ctx, call_si, name );
		}
	}
}

void Class::UnsetPropertiesFromParent( GSE_CALLABLE ) {
	ASSERT_NOLOG( m_parent_class, "parent class not set" );
	for ( const auto& it : m_parent_class->m_properties ) {
		UnsetPropertyFromParent( ctx, call_si, it.first );
	}
}

void Class::UpdateObject( GSE_CALLABLE, dom::Object* const object ) {
	for ( const auto& property : m_properties ) {
		if ( m_is_master ) {
			const auto it = s_name_to_modifier.find( property.first );
			if ( it != s_name_to_modifier.end() ) {
				ASSERT_NOLOG( m_subclasses.find( it->second ) != m_subclasses.end(), "subclass not found" );
				//m_subclasses.at( it->second )->UpdateObject()
				continue;
			}
		}
		object->SetPropertyFromClass( ctx, call_si, property.first, property.second );
	}
}

void Class::AddChildClass( GSE_CALLABLE, Class* const cls ) {
	ASSERT_NOLOG( m_child_classes.find( cls ) == m_child_classes.end(), "child class already exists" );
	m_child_classes.insert( cls );
	cls->SetPropertiesFromParent( ctx, call_si );
}

void Class::RemoveChildClass( GSE_CALLABLE, Class* const cls ) {
	ASSERT_NOLOG( m_child_classes.find( cls ) != m_child_classes.end(), "child class not found" );
	cls->UnsetPropertiesFromParent( ctx, call_si );
	m_child_classes.erase( cls );
}

void Class::SetProperties( GSE_CALLABLE, const properties_t& properties ) {
	for ( const auto& it : properties ) {
		m_local_properties.insert_or_assign( it.first, it.second );
		SetProperty( ctx, call_si, it.first, it.second );
	}
}

void Class::SetParentClass( GSE_CALLABLE, const std::string& name ) {
	auto* cls = m_ui->GetClass( name );
	if ( !cls ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Class '" + name + "' does not exist" );
	}
	if ( m_parent_class != cls ) {
		if ( m_parent_class ) {
			m_parent_class->RemoveChildClass( ctx, call_si, this );
		}
		m_parent_class = cls;
		if ( cls != nullptr ) {
			m_parent_class->AddChildClass( ctx, call_si, this );
		}
	}
}

}
