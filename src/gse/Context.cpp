#include "Context.h"

#include "Exception.h"

namespace gse {

Context::Context( const Context* parent_context, const source_lines_t& source_lines, const si_t& si )
	: m_parent_context( parent_context )
	, m_source_lines( source_lines )
	, m_si( si ) {
}

Context::~Context() {

}

const Value Context::GetVariable( const std::string& name, const si_t* si ) {
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		Log( "GetVariable( " + name + " )" );
		return it->second;
	}
	const auto ref_it = m_ref_contexts.find( name );
	if ( ref_it != m_ref_contexts.end() ) {
		return ref_it->second->GetVariable( name, si );
	}
	THROW( "variable '" + name + "' not found" );
}

void Context::CreateVariable( const std::string& name, const Value& value, const si_t* si ) {
	ASSERT( m_variables.find( name ) == m_variables.end(), "variable '" + name + "' already exists" );
	Log( "CreateVariable( " + name + ", " + value.ToString() + " )" );
	m_variables.insert_or_assign( name, value );
}

void Context::UpdateVariable( const std::string& name, const Value& value, const si_t* si ) {
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		Log( "UpdateVariable( " + name + ", " + value.ToString() + " )" );
		it->second = value;
		return;
	}
	const auto ref_it = m_ref_contexts.find( name );
	if ( ref_it != m_ref_contexts.end() ) {
		ref_it->second->UpdateVariable( name, value, si );
		return;
	}
	throw Exception( "ReferenceError", "Variable '" + name + "' is not defined", this, *si );
	THROW( "variable '" + name + "' not found" );
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

void Context::JoinContext( Context* const other ) const {
	for ( const auto& it : m_variables ) {
		other->m_variables.insert_or_assign( it.first, it.second );
	}
}

}
