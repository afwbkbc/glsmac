#include "Context.h"

#include <iostream>

#include "ChildContext.h"
#include "gse/Exception.h"
#include "common/Common.h"
#include "gse/type/Callable.h"
#include "gse/type/Undefined.h"

namespace gse {
namespace context {

Context::Context( gse::GSE* gse )
	: m_gse( gse ) {
}

Context::~Context() {}

GSE* Context::GetGSE() const {
	return m_gse;
}

void Context::Begin() {
	IncRefs();
}

const bool Context::End() {
	std::vector< std::string > callables_to_remove = {};
	for ( const auto& v : m_variables ) {
		if ( v.second.value.Get()->type == type::Type::T_CALLABLE ) {
			auto* c = (type::Callable*)v.second.value.Get();
			if ( c->m_cleanup_needed ) {
				callables_to_remove.push_back( v.first );
			}
		}
	}
	for ( const auto& c : callables_to_remove ) {
		m_variables.erase( c );
	}
	return DecRefs();
}

void Context::IncRefs() {
	m_refs++;
}

const bool Context::DecRefs() {
	ASSERT_NOLOG( m_refs > 0, "refs not positive" );
	if ( !--m_refs ) {
		DELETE( this );
		return true;
	}
	return false;
}

void Context::WithRefs( const std::function< void() >& f ) {
	IncRefs();
	try {
		f();
	}
	catch ( const std::exception& e ) {
		DecRefs();
		throw;
	}
	DecRefs();
}

const gse::Value Context::WithRefsV( const std::function< const gse::Value() >& f ) {
	IncRefs();
	auto result = VALUE( type::Undefined );
	try {
		result = f();
	}
	catch ( const gse::Exception& e ) {
		std::cout << e.ToString() << std::endl;
		DecRefs();
		throw;
	}
	catch ( const std::exception& e ) {
		DecRefs();
		throw;
	}
	DecRefs();
	return result;
}

const bool Context::HasVariable( const std::string& name ) {
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		return true;
	}
	const auto ref_it = m_ref_contexts.find( name );
	if ( ref_it != m_ref_contexts.end() ) {
		return true;
	}
	return false;
}

const Value Context::GetVariable( const std::string& name, const si_t& si, gse::ExecutionPointer& ep ) {
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		return it->second.value;
	}
	const auto ref_it = m_ref_contexts.find( name );
	if ( ref_it != m_ref_contexts.end() ) {
		ASSERT_NOLOG( ref_it->second != this, "unexpected ref context recursion (was this context freed while in use?)" );
		return ref_it->second->GetVariable( name, si, ep );
	}
	throw Exception( EC.REFERENCE_ERROR, "Variable '" + name + "' is not defined", this, si, ep );
}

void Context::SetVariable( const std::string& name, const var_info_t& var_info ) {
	m_variables.insert_or_assign( name, var_info );
}

void Context::CreateVariable( const std::string& name, const Value& value, const si_t& si, gse::ExecutionPointer& ep ) {
	if ( m_variables.find( name ) != m_variables.end() ) {
		throw Exception( EC.INVALID_ASSIGNMENT, "Variable '" + name + "' already exists", this, si, ep );
	}
	m_variables.insert_or_assign(
		name, var_info_t{
			value,
			false,
		}
	);
}

void Context::CreateConst( const std::string& name, const Value& value, const si_t& si, gse::ExecutionPointer& ep ) {
	if ( m_variables.find( name ) != m_variables.end() ) {
		throw Exception( EC.INVALID_ASSIGNMENT, "Variable '" + name + "' already exists", this, si, ep );
	}
	m_variables.insert_or_assign(
		name, var_info_t{
			value,
			true
		}
	);
}

void Context::UpdateVariable( const std::string& name, const Value& value, const si_t& si, gse::ExecutionPointer& ep ) {
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		if ( it->second.is_const ) {
			throw Exception( EC.INVALID_ASSIGNMENT, "Can't change value of const '" + name + "'", this, si, ep );
		}
		it->second.value = value;
		return;
	}
	const auto ref_it = m_ref_contexts.find( name );
	if ( ref_it != m_ref_contexts.end() ) {
		ref_it->second->UpdateVariable( name, value, si, ep );
		return;
	}
	throw Exception( EC.REFERENCE_ERROR, "Variable '" + name + "' is not defined", this, si, ep );
}

void Context::DestroyVariable( const std::string& name, const si_t& si, gse::ExecutionPointer& ep ) {
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		m_variables.erase( it );
		return;
	}
	throw Exception( EC.REFERENCE_ERROR, "Variable '" + name + "' is not defined", this, si, ep );
}

void Context::CreateBuiltin( const std::string& name, const Value& value, gse::ExecutionPointer& ep ) {
	CreateConst( "#" + name, value, {}, ep );
}

void Context::PersistValue( const Value& value ) {
	ASSERT_NOLOG( m_persisted_values.find( value.Get() ) == m_persisted_values.end(), "value already persisted" );
	m_persisted_values.insert(
		{
			value.Get(),
			value
		}
	);
	/*// this context chain must live until all variables are unpersisted
	Context* ctx = this;
	while ( ctx ) {
		ctx->IncRefs();
		ctx = ctx->GetParentContext();
	}*/
	IncRefs();
}

void Context::UnpersistValue( const Value& value ) {
	ASSERT_NOLOG( m_persisted_values.find( value.Get() ) != m_persisted_values.end(), "value was not persisted" );
	m_persisted_values.erase( value.Get() );
	/*Context* ctx = this;
	while ( ctx ) {
		ctx->DecRefs();
		ctx = ctx->GetParentContext();
	}*/
	DecRefs();
}

void Context::UnpersistValue( const type::Type* type ) {
	ASSERT_NOLOG( m_persisted_values.find( type ) != m_persisted_values.end(), "value was not persisted" );
	m_persisted_values.erase( type );
	DecRefs();
}

ChildContext* const Context::ForkContext(
	GSE_CALLABLE,
	const bool is_traceable,
	const std::vector< std::string > parameters,
	const type::function_arguments_t& arguments
) {
	if ( parameters.size() != arguments.size() ) {
		throw Exception( EC.INVALID_CALL, "Expected " + std::to_string( parameters.size() ) + " arguments, found " + std::to_string( arguments.size() ), this, si, ep );
	}
	NEWV( result, ChildContext, m_gse, this, si, is_traceable );
	// functions have access to parent variables
	for ( auto& it : m_ref_contexts ) {
		result->m_ref_contexts.insert_or_assign( it.first, it.second );
	}
	for ( auto& it : m_variables ) {
		result->m_ref_contexts.insert_or_assign( it.first, this );
	}
	for ( size_t i = 0 ; i < parameters.size() ; i++ ) { // inject passed arguments
		result->CreateVariable( parameters[ i ], arguments[ i ], si, ep );
	}
	return result;
}

void Context::Clear() {
	WithRefs(
		[ this ]() {
			const auto children = m_child_contexts;
			for ( const auto& c : children ) {
				c->Clear();
			}
			m_persisted_values.clear();
			m_variables.clear();
		}
	);
}

void Context::AddChildContext( ChildContext* const child ) {
	ASSERT_NOLOG( m_child_contexts.find( child ) == m_child_contexts.end(), "child context already added" );
	IncRefs();
	m_child_contexts.insert( child );
}

void Context::RemoveChildContext( ChildContext* const child ) {
	if ( !m_child_contexts.empty() ) {
		ASSERT_NOLOG( m_child_contexts.find( child ) != m_child_contexts.end(), "child context not found" );
		m_child_contexts.erase( child );
		DecRefs();
	}
}

void Context::AddChildObject( type::Type* const child ) {
	ASSERT_NOLOG( m_child_objects.find( child ) == m_child_objects.end(), "child context already added" );
	IncRefs();
	m_child_objects.insert( child );
}

void Context::RemoveChildObject( type::Type* const child ) {
	if ( !m_child_objects.empty() ) {
		ASSERT_NOLOG( m_child_objects.find( child ) != m_child_objects.end(), "child context not found" );
		m_child_objects.erase( child );
		DecRefs();
	}
}

}
}
