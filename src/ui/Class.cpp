#include "Class.h"

#include "ui/UI.h"
#include "ui/dom/Object.h"
#include "gse/type/Array.h"

namespace ui {

static const std::unordered_map< std::string, class_modifier_t > s_name_to_modifier = {
	{ "_focus", CM_FOCUS },
	{ "_hover", CM_HOVER },
	{ "_active", CM_ACTIVE },
};

static const std::unordered_map< class_modifier_t, std::string > s_modifier_to_name = {
	{ CM_FOCUS, "_focus" },
	{ CM_HOVER, "_hover" },
	{ CM_ACTIVE, "_active" },
};

Class::Class( const UI* const ui, const std::string& name, const bool is_master )
	: m_ui( ui )
	, m_name( name )
	, m_is_master( is_master ) {
	if ( m_is_master ) {
		m_subclasses = {
			{ CM_HOVER, new Class( ui, name + "._hover", false ) },
			{ CM_ACTIVE, new Class( ui, name + "._active", false ) },
			{ CM_FOCUS, new Class( ui, name + "._focus", false ) },
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

const std::string& Class::GetName() const {
	return m_name;
}

const std::pair< gse::Value, class_modifier_t > Class::GetProperty( const std::string& key, const class_modifiers_t& modifiers ) const {
	auto v = std::make_pair( VALUE( gse::type::Undefined ), CM_NONE );

	if ( m_is_master ) {
		// search in modifiers
		for ( const auto& m : modifiers ) {
			ASSERT_NOLOG( m_subclasses.find( m ) != m_subclasses.end(), "subclass not found" );
			v = m_subclasses.at( m )->GetProperty( key, {} );
			if ( v.first.Get()->type != gse::type::Type::T_UNDEFINED ) {
				v.second = m;
				return v;
			}
		}
	}
	else {
		ASSERT_NOLOG( modifiers.empty(), "modifiers passed to non-master class" );
	}

	// search in own properties
	const auto& it = m_properties.find( key );
	if ( it != m_properties.end() ) {
		v.first = it->second;
		v.second = CM_NONE;
		return v;
	}

	// nothing found
	return v;
}

const properties_t& Class::GetProperties() const {
	return m_properties;
}

void Class::AddObject( GSE_CALLABLE, dom::Object* object, const class_modifiers_t& modifiers ) {
	ASSERT_NOLOG( m_objects.find( object ) == m_objects.end(), "object already exists in class" );
	m_objects.insert( { object, modifiers } );
	if ( m_is_master ) {
		for ( const auto& m : modifiers ) {
			ASSERT_NOLOG( m_subclasses.find( m ) != m_subclasses.end(), "subclass not found" );
			m_subclasses.at( m )->AddObject( GSE_CALL, object, {} );
		}
		UpdateObject( GSE_CALL, object, CM_NONE );
	}
}

void Class::RemoveObject( GSE_CALLABLE, dom::Object* object ) {
	ASSERT_NOLOG( m_objects.find( object ) != m_objects.end(), "object not found in class" );
	if ( m_is_master ) {
		for ( const auto& m : m_objects.at( object ) ) {
			ASSERT_NOLOG( m_subclasses.find( m ) != m_subclasses.end(), "subclass not found" );
			m_subclasses.at( m )->RemoveObject( GSE_CALL, object );
		}
	}
	if ( m_is_master ) {
		for ( const auto& it : m_properties ) {
			if ( s_name_to_modifier.find( it.first ) == s_name_to_modifier.end() ) {
				object->UnsetPropertyFromClass( GSE_CALL, it.first );
			}
		}
	}
	m_objects.erase( object );
}

void Class::UpdateObject( GSE_CALLABLE, dom::Object* const object, const class_modifier_t modifier ) {
	for ( const auto& property : m_properties ) {
		if ( s_name_to_modifier.find( property.first ) != s_name_to_modifier.end() ) {
			ASSERT_NOLOG( m_is_master, "modifier received by non-master" );
			continue;
		}
		object->SetPropertyFromClass( GSE_CALL, property.first, property.second, modifier );
	}
	if ( m_is_master ) {
		ASSERT_NOLOG( m_objects.find( object ) != m_objects.end(), "object not found" );
		for ( const auto& m : m_objects.at( object ) ) {
			ASSERT_NOLOG( m_subclasses.find( m ) != m_subclasses.end(), "subclass not found" );
			m_subclasses.at( m )->UpdateObject( GSE_CALL, object, m );
		}
	}
}

void Class::AddObjectModifier( GSE_CALLABLE, dom::Object* object, const class_modifier_t modifier ) {
	ASSERT_NOLOG( m_objects.find( object ) != m_objects.end(), "object not found" );
	auto& modifiers = m_objects.at( object );
	if ( modifiers.find( modifier ) == modifiers.end() ) {
		ASSERT_NOLOG( m_subclasses.find( modifier ) != m_subclasses.end(), "subclass not found" );
		modifiers.insert( modifier );
		const auto& cls = m_subclasses.at( modifier );
		cls->AddObject( GSE_CALL, object, {} );
		cls->UpdateObject( GSE_CALL, object, modifier );
	}
}

void Class::RemoveObjectModifier( GSE_CALLABLE, dom::Object* object, const class_modifier_t modifier ) {
	ASSERT_NOLOG( m_objects.find( object ) != m_objects.end(), "object not found" );
	auto& modifiers = m_objects.at( object );
	if ( modifiers.find( modifier ) != modifiers.end() ) {
		ASSERT_NOLOG( m_subclasses.find( modifier ) != m_subclasses.end(), "subclass not found" );
		modifiers.erase( modifier );
		const auto& cls = m_subclasses.at( modifier );
		m_subclasses.at( modifier )->RemoveObject( GSE_CALL, object );
		for ( const auto& p : cls->m_properties ) {
			auto v = VALUE( gse::type::Undefined );
			// check if this property exists outside of subclass
			for ( auto it = modifiers.rbegin() ; it != modifiers.rend() ; it++ ) {
				const auto& pp = m_subclasses.at( *it )->m_properties;
				const auto& it_p = pp.find( p.first );
				if ( it_p != pp.end() && it_p->second.Get()->type != gse::type::Type::T_UNDEFINED ) {
					v = it_p->second;
					break;
				}
			}
			if ( v.Get()->type == gse::type::Type::T_UNDEFINED ) {
				const auto& it_p = m_properties.find( p.first );
				if ( it_p != m_properties.end() && it_p->second.Get()->type != gse::type::Type::T_UNDEFINED ) {
					v = it_p->second;
				}
			}
			if ( v.Get()->type == gse::type::Type::T_UNDEFINED ) {
				object->UnsetPropertyFromClass( GSE_CALL, p.first );
			}
			else {
				object->SetPropertyFromClass( GSE_CALL, p.first, v, CM_NONE );
			}
		}
	}
}

const gse::Value Class::Wrap( const bool dynamic ) {
	gse::type::object_properties_t properties = {
		{
			"extend",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( name, 0, String );
				SetParentClass( GSE_CALL, name );
				return Wrap( true );
			} )
		},
		{
			"set",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( properties, 0, Object );
				SetProperties( GSE_CALL, properties );
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
				UnsetProperties( GSE_CALL, names );
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
	SetProperty( GSE_CALL, key, value );
}

void Class::WrapSetStatic( gse::Wrappable* wrapobj, const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& call_si ) {
	ASSERT_NOLOG( wrapobj, "wrapobj not set" );
	( (Class*)wrapobj )->WrapSet( key, value, GSE_CALL );
}

void Class::SetProperty( gse::context::Context* ctx, const gse::si_t& call_si, const std::string& name, const gse::Value& value ) {
	const auto& it_old = m_properties.find( name );
	if ( it_old == m_properties.end() || it_old->second != value ) {
		const auto& it = s_name_to_modifier.find( name );
		if ( it != s_name_to_modifier.end() ) {
			ASSERT_NOLOG( m_subclasses.find( it->second ) != m_subclasses.end(), "subclass not found" );
			if ( value.Get()->type != gse::type::Type::T_OBJECT ) {
				GSE_ERROR( gse::EC.UI_ERROR, "Class " + name + " definition must be Object, found " + value.GetTypeString() + ": " + value.ToString() );
			}
			const auto& cls = m_subclasses.at( it->second );
			for ( const auto& p : ((gse::type::Object*)value.Get())->value ) {
				cls->SetProperty( GSE_CALL, p.first, p.second );
			}
		}
		m_properties.insert_or_assign( name, value );
		for ( const auto& obj : m_wrapobjs ) {
			obj->value.insert_or_assign( name, value );
		}
		for ( const auto& cls : m_child_classes ) {
			cls->SetPropertyFromParent( GSE_CALL, name, value );
		}
		for ( const auto& object : m_objects ) {
			object.first->SetPropertyFromClass( GSE_CALL, name, value, CM_NONE );
		}
	}
}

void Class::SetPropertyFromParent( GSE_CALLABLE, const std::string& name, const gse::Value& value ) {
	const auto& local_it = m_local_properties.find( name );
	if ( local_it == m_local_properties.end() ) { // local properties should have priority
		SetProperty( GSE_CALL, name, value );
	}
}

void Class::SetPropertiesFromParent( GSE_CALLABLE ) {
	ASSERT_NOLOG( m_parent_class, "parent class not set" );
	for ( const auto& it : m_parent_class->m_properties ) {
		SetPropertyFromParent( GSE_CALL, it.first, it.second );
	}
}

void Class::UnsetProperty( GSE_CALLABLE, const std::string& name ) {
	const auto& it = m_properties.find( name );
	if ( it != m_properties.end() ) {
		m_properties.erase( it );
		for ( const auto& cls : m_child_classes ) {
			cls->UnsetPropertyFromParent( GSE_CALL, name );
		}
		for ( const auto& object : m_objects ) {
			object.first->UnsetPropertyFromClass( GSE_CALL, name );
		}
	}
}

void Class::UnsetPropertyFromParent( GSE_CALLABLE, const std::string& name ) {
	const auto& local_it = m_local_properties.find( name );
	if ( local_it == m_local_properties.end() ) { // otherwise it would have been overridden by local property anyway
		UnsetProperty( GSE_CALL, name );
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
					SetProperty( GSE_CALL, name, parent_it->second );
					continue;
				}
			}
			UnsetProperty( GSE_CALL, name );
		}
	}
}

void Class::UnsetPropertiesFromParent( GSE_CALLABLE ) {
	ASSERT_NOLOG( m_parent_class, "parent class not set" );
	for ( const auto& it : m_parent_class->m_properties ) {
		UnsetPropertyFromParent( GSE_CALL, it.first );
	}
}

void Class::AddChildClass( GSE_CALLABLE, Class* const cls ) {
	ASSERT_NOLOG( m_child_classes.find( cls ) == m_child_classes.end(), "child class already exists" );
	m_child_classes.insert( cls );
	cls->SetPropertiesFromParent( GSE_CALL );
}

void Class::RemoveChildClass( GSE_CALLABLE, Class* const cls ) {
	ASSERT_NOLOG( m_child_classes.find( cls ) != m_child_classes.end(), "child class not found" );
	cls->UnsetPropertiesFromParent( GSE_CALL );
	m_child_classes.erase( cls );
}

void Class::SetProperties( GSE_CALLABLE, const properties_t& properties ) {
	for ( const auto& it : properties ) {
		m_local_properties.insert_or_assign( it.first, it.second );
		SetProperty( GSE_CALL, it.first, it.second );
	}
}

void Class::SetParentClass( GSE_CALLABLE, const std::string& name ) {
	auto* cls = m_ui->GetClass( name );
	if ( !cls ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Class '" + name + "' does not exist" );
	}
	if ( m_parent_class != cls ) {
		if ( m_parent_class ) {
			m_parent_class->RemoveChildClass( GSE_CALL, this );
		}
		m_parent_class = cls;
		if ( cls != nullptr ) {
			m_parent_class->AddChildClass( GSE_CALL, this );
		}
	}
}

}
