#include "Context.h"

namespace gse {

Context::Context() {
	m_scopes.push_back( new Scope() ); // global scope
}

Context::~Context() {
	for ( const auto& it : m_scopes ) {
		delete it;
	}
}

const Value Context::GetVariable( const std::string& name ) {
	Log( "GetVariable( " + name + " )" );
	Scope::variables_t::const_iterator it;
	for ( auto scope = m_scopes.rbegin() ; scope != m_scopes.rend() ; scope++ ) {
		it = ( *scope )->m_variables.find( name );
		if ( it != ( *scope )->m_variables.end() ) {
			return it->second;
		}
	}
	ASSERT( false, "variable '" + name + "' not found" );
}

void Context::CreateVariable( const std::string& name, const Value& value ) {
	Log( "CreateVariable( " + name + ", " + value.ToString() + " )" );
	auto* scope = m_scopes.back();
	ASSERT( scope->m_variables.find( name ) == scope->m_variables.end(), "variable '" + name + "' already exists" );
	scope->m_variables.insert_or_assign( name, value );
}

void Context::UpdateVariable( const std::string& name, const Value& value ) {
	Log( "UpdateVariable( " + name + ", " + value.ToString() + " )" );
	Scope::variables_t::iterator it;
	for ( auto scope = m_scopes.rbegin() ; scope != m_scopes.rend() ; scope++ ) {
		it = ( *scope )->m_variables.find( name );
		if ( it != ( *scope )->m_variables.end() ) {
			it->second = value;
			return;
		}
	}
	ASSERT( false, "variable '" + name + "' not found" );
}

void Context::PushScope() {
	Log( "PushScope()" );
	m_scopes.push_back( new Scope() );
}

void Context::PopScope() {
	Log( "PopScope()" );
	ASSERT( m_scopes.size() > 1, "attempted to destroy global scope" );
	delete m_scopes.back();
	m_scopes.pop_back();
}

Context* const Context::CreateFunctionScope(
	const std::vector< std::string > parameters,
	const type::Callable::function_arguments_t& arguments
) const {
	ASSERT( parameters.size() == arguments.size(), "expected " + std::to_string( parameters.size() ) + " arguments, found " + std::to_string( arguments.size() ) );
	auto* result = new Context();
	result->m_scopes[ 0 ]->m_variables = m_scopes[ 0 ]->m_variables; // functions have access to parent variables but nothing else
	result->PushScope(); // functions can have local variables
	for ( size_t i = 0 ; i < parameters.size() ; i++ ) { // inject passed arguments
		result->CreateVariable( parameters[ i ], arguments[ i ] );
	}
	return result;
}

}
