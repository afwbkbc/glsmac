#include "Context.h"

#include "ChildContext.h"
#include "gse/Exception.h"
#include "common/Common.h"

namespace gse {
namespace context {

Context::Context( gse::GSE* gse )
	: m_gse( gse ) {
}

Context::~Context() {
}

GSE* Context::GetGSE() const {
	return m_gse;
}

void Context::IncRefs() {
	m_refs++;
}

void Context::DecRefs() {
	ASSERT_NOLOG( m_refs > 0, "refs not positive" );
	if ( !--m_refs ) {
		DELETE( this );
	}
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

const Value Context::GetVariable( const std::string& name, const si_t* si ) {
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		return it->second.value;
	}
	const auto ref_it = m_ref_contexts.find( name );
	if ( ref_it != m_ref_contexts.end() ) {
		ASSERT_NOLOG( ref_it->second != this, "unexpected ref context recursion (was this context freed while in use?)" );
		return ref_it->second->GetVariable( name, si );
	}
	throw Exception( EC.REFERENCE_ERROR, "Variable '" + name + "' is not defined", this, *si );
}

void Context::SetVariable( const std::string& name, const var_info_t& var_info ) {
	m_variables.insert_or_assign( name, var_info );
}

void Context::CreateVariable( const std::string& name, const Value& value, const si_t* si ) {
	if ( m_variables.find( name ) != m_variables.end() ) {
		throw Exception( EC.INVALID_ASSIGNMENT, "Variable '" + name + "' already exists", this, *si );
	}
	m_variables.insert_or_assign(
		name, var_info_t{
			value,
			false,
		}
	);
}

void Context::CreateConst( const std::string& name, const Value& value, const si_t* si ) {
	if ( m_variables.find( name ) != m_variables.end() ) {
		throw Exception( EC.INVALID_ASSIGNMENT, "Variable '" + name + "' already exists", this, *si );
	}
	m_variables.insert_or_assign(
		name, var_info_t{
			value,
			true
		}
	);
}

void Context::UpdateVariable( const std::string& name, const Value& value, const si_t* si ) {
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		if ( it->second.is_const ) {
			throw Exception( EC.INVALID_ASSIGNMENT, "Can't change value of const '" + name + "'", this, *si );
		}
		it->second.value = value;
		return;
	}
	const auto ref_it = m_ref_contexts.find( name );
	if ( ref_it != m_ref_contexts.end() ) {
		ref_it->second->UpdateVariable( name, value, si );
		return;
	}
	throw Exception( EC.REFERENCE_ERROR, "Variable '" + name + "' is not defined", this, *si );
}

void Context::DestroyVariable( const std::string& name, const si_t* si ) {
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		m_variables.erase( it );
		return;
	}
	throw Exception( EC.REFERENCE_ERROR, "Variable '" + name + "' is not defined", this, *si );
}

void Context::CreateBuiltin( const std::string& name, const Value& value ) {
	CreateConst( "#" + name, value, nullptr );
}

void Context::PersistValue( const Value& value ) {
	ASSERT_NOLOG( m_persisted_values.find( value.Get() ) == m_persisted_values.end(), "value already persisted" );
	m_persisted_values.insert(
		{
			value.Get(),
			value
		}
	);
	// this context chain must live until all variables are unpersisted
	Context* ctx = this;
	while ( ctx ) {
		ctx->IncRefs();
		ctx = ctx->GetParentContext();
	}
}

void Context::UnpersistValue( const Value& value ) {
	ASSERT_NOLOG( m_persisted_values.find( value.Get() ) != m_persisted_values.end(), "value was not persisted" );
	m_persisted_values.erase( value.Get() );
	Context* ctx = this;
	while ( ctx ) {
		ctx->DecRefs();
		ctx = ctx->GetParentContext();
	}
}

void Context::UnpersistValue( const type::Type* type ) {
	ASSERT_NOLOG( m_persisted_values.find( type ) != m_persisted_values.end(), "value was not persisted" );
	m_persisted_values.erase( type );
	DecRefs();
}

ChildContext* const Context::ForkContext(
	Context* caller_context,
	const si_t& call_si,
	const bool is_traceable,
	const std::vector< std::string > parameters,
	const type::function_arguments_t& arguments
) {
	if ( parameters.size() != arguments.size() ) {
		throw Exception( EC.INVALID_CALL, "Expected " + std::to_string( parameters.size() ) + " arguments, found " + std::to_string( arguments.size() ), this, call_si );
	}
	NEWV( result, ChildContext, m_gse, this, caller_context, call_si, is_traceable );
	// functions have access to parent variables
	for ( auto& it : m_ref_contexts ) {
		result->m_ref_contexts.insert_or_assign( it.first, it.second );
	}
	for ( auto& it : m_variables ) {
		result->m_ref_contexts.insert_or_assign( it.first, this );
	}
	for ( size_t i = 0 ; i < parameters.size() ; i++ ) { // inject passed arguments
		result->CreateVariable( parameters[ i ], arguments[ i ], &call_si );
	}
	return result;
}

}
}
