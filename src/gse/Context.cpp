#include "Context.h"

namespace gse {

Context::Context( const Context* parent_context, const source_lines_t& source_lines, const si_t& si )
	: m_parent_context( parent_context )
	, m_source_lines( source_lines )
	, m_si( si ) {
}

Context::~Context() {

}

const Value Context::GetVariable( const std::string& name ) {
	Log( "GetVariable( " + name + " )" );
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		return it->second;
	}
	const auto ref_it = m_ref_contexts.find( name );
	if ( ref_it != m_ref_contexts.end() ) {
		return ref_it->second->GetVariable( name );
	}
	THROW( "variable '" + name + "' not found" );
}

void Context::CreateVariable( const std::string& name, const Value& value ) {
	Log( "CreateVariable( " + name + ", " + value.ToString() + " )" );
	ASSERT( m_variables.find( name ) == m_variables.end(), "variable '" + name + "' already exists" );
	m_variables.insert_or_assign( name, value );
}

void Context::UpdateVariable( const std::string& name, const Value& value, bool create_if_missing ) {
	Log( "UpdateVariable( " + name + ", " + value.ToString() + " )" );
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		it->second = value;
		return;
	}
	const auto ref_it = m_ref_contexts.find( name );
	if ( ref_it != m_ref_contexts.end() ) {
		ref_it->second->UpdateVariable( name, value );
		return;
	}
	if ( create_if_missing ) {
		CreateVariable( name, value );
	}
	else {
		THROW( "variable '" + name + "' not found" );
	}
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
) {
	ASSERT( parameters.size() == arguments.size(), "expected " + std::to_string( parameters.size() ) + " arguments, found " + std::to_string( arguments.size() ) );
	NEWV( result, Context, this, m_source_lines, call_si );
	// functions have access to parent variables
	for ( auto& it : m_variables ) {
		result->m_ref_contexts.insert_or_assign( it.first, this );
	}
	for ( size_t i = 0 ; i < parameters.size() ; i++ ) { // inject passed arguments
		result->CreateVariable( parameters[ i ], arguments[ i ] );
	}
	return result;
}

void Context::JoinContext( Context* const other ) const {
	for ( const auto& it : m_variables ) {
		other->UpdateVariable( it.first, it.second, true );
	}
}

}
