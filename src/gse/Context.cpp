#include "Context.h"

#include "gse/Exception.h"

namespace gse {

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
			false
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

ChildContext* const Context::ForkContext(
	Context* caller_context,
	const si_t& call_si,
	const bool is_traceable,
	const std::vector< std::string > parameters,
	const type::Callable::function_arguments_t& arguments
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
