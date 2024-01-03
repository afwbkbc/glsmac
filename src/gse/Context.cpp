#include "Context.h"

namespace gse {

Context::Context( const Context* parent_context, const source_lines_t& source_lines, const si_t& si )
	: m_parent_context( parent_context )
	, m_source_lines( source_lines )
	, m_si( si ) {
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
	THROW( "variable '" + name + "' not found" );
}

void Context::CreateVariable( const std::string& name, const Value& value ) {
	Log( "CreateVariable( " + name + ", " + value.ToString() + " )" );
	auto* scope = m_scopes.back();
	ASSERT( scope->m_variables.find( name ) == scope->m_variables.end(), "variable '" + name + "' already exists" );
	scope->m_variables.insert_or_assign( name, value );
}

void Context::UpdateVariable( const std::string& name, const Value& value, bool create_if_missing ) {
	Log( "UpdateVariable( " + name + ", " + value.ToString() + " )" );
	Scope::variables_t::iterator it;
	for ( auto scope = m_scopes.rbegin() ; scope != m_scopes.rend() ; scope++ ) {
		it = ( *scope )->m_variables.find( name );
		if ( it != ( *scope )->m_variables.end() ) {
			it->second = value;
			return;
		}
	}
	if ( create_if_missing ) {
		CreateVariable( name, value );
	}
	else {
		THROW( "variable '" + name + "' not found" );
	}
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

const size_t Context::GetScopeDepth() const {
	return m_scopes.size();
}

const Context* Context::GetParentContext() const {
	return m_parent_context;
}

const si_t& Context::GetSI() const {
	return m_si;
}

void Context::AddSourceLine( const std::string& source_line ) {
	m_source_lines.push_back( source_line );
}

void Context::AddSourceLines( const source_lines_t& source_lines ) {
	m_source_lines.insert( m_source_lines.begin(), source_lines.begin(), source_lines.end() );
}

const std::string Context::GetSourceLine( const size_t line_num ) const {
	if ( !( line_num > 0 && line_num <= m_source_lines.size() ) ) {
		return "<overflow>";
	}
	ASSERT( line_num > 0 && line_num <= m_source_lines.size(), "source line overflow" );
	return m_source_lines.at( line_num - 1 );
}

const Context::source_lines_t& Context::GetSourceLines() const {
	return m_source_lines;
}

Context* const Context::ForkContext(
	const si_t& call_si,
	const std::vector< std::string > parameters,
	const type::Callable::function_arguments_t& arguments
) const {
	ASSERT( parameters.size() == arguments.size(), "expected " + std::to_string( parameters.size() ) + " arguments, found " + std::to_string( arguments.size() ) );
	auto* result = new Context( this, m_source_lines, call_si );
	result->m_scopes[ 0 ]->m_variables = m_scopes[ 0 ]->m_variables; // functions have access to parent variables but nothing else
	result->PushScope(); // functions can have local variables
	for ( size_t i = 0 ; i < parameters.size() ; i++ ) { // inject passed arguments
		result->CreateVariable( parameters[ i ], arguments[ i ] );
	}
	return result;
}

void Context::JoinContext( Context* const other ) const {
	for ( const auto& scope : m_scopes ) {
		for ( const auto& it : scope->m_variables ) {
			other->UpdateVariable( it.first, it.second, true );
		}
	}
}

}
