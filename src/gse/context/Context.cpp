#include "Context.h"

#include "ChildContext.h"
#include "gse/GSE.h"
#include "gse/Exception.h"
#include "common/Common.h"
#include "gse/value/Callable.h"
#include "gse/value/ValueRef.h"
#include "gc/Space.h"
#include "util/LogHelper.h"

static const std::unordered_set< std::string > s_variable_reserved_words = {
	"this",
	"parent",
};

namespace gse {
namespace context {

Context::Context( gse::GSE* gse )
	: gc::Object( gse->GetGCSpace() )
	, m_gse( gse ) {}

Context::~Context() {
	for ( const auto& child_context : m_child_contexts ) {
		child_context->Detach();
	}
}

GSE* Context::GetGSE() const {
	return m_gse;
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

Value* const Context::GetVariable( const std::string& name, const si_t& si, gse::ExecutionPointer& ep ) {
	CHECKACCUM( m_gse->GetGCSpace() );
	if ( name == "this" && m_this ) {
		return m_this;
	}
	else if ( name == "parent" && m_parent ) {
		return m_parent;
	}
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		return it->second.value;
	}
	const auto ref_it = m_ref_contexts.find( name );
	if ( ref_it != m_ref_contexts.end() ) {
		const auto ref = ref_it->second;
		ASSERT( ref != this, "unexpected ref context recursion (was this context freed while in use?)" );
		return ref->GetVariable( name, si, ep );
	}
	throw Exception( EC.REFERENCE_ERROR, "Variable '" + name + "' is not defined", CONTEXT_GSE_CALL );
}

void Context::CreateVariable( const std::string& name, Value* const value, CONTEXT_GSE_CALLABLE ) {
	ASSERT( value, "value is null" );
	CheckVariableName( name, CONTEXT_GSE_CALL );
	if ( m_variables.find( name ) != m_variables.end() ) {
		throw Exception( EC.INVALID_ASSIGNMENT, "Variable '" + name + "' already exists", CONTEXT_GSE_CALL );
	}
	m_variables.insert_or_assign(
		name, var_info_t{
			value,
			false,
		}
	);
}

void Context::CreateConst( const std::string& name, Value* const value, CONTEXT_GSE_CALLABLE ) {
	ASSERT( value, "value is null" );
	CheckVariableName( name, CONTEXT_GSE_CALL );
	if ( m_variables.find( name ) != m_variables.end() ) {
		throw Exception( EC.INVALID_ASSIGNMENT, "Variable '" + name + "' already exists", CONTEXT_GSE_CALL );
	}
	m_variables.insert_or_assign(
		name, var_info_t{
			value,
			true
		}
	);
}

void Context::UpdateVariable( const std::string& name, Value* const value, CONTEXT_GSE_CALLABLE ) {
	ASSERT( value, "value is null" );
	CheckVariableName( name, CONTEXT_GSE_CALL );
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		if ( it->second.is_const ) {
			throw Exception( EC.INVALID_ASSIGNMENT, "Can't change value of const '" + name + "'", CONTEXT_GSE_CALL );
		}
		it->second.value = value;
		return;
	}
	const auto ref_it = m_ref_contexts.find( name );
	if ( ref_it != m_ref_contexts.end() ) {
		ref_it->second->UpdateVariable( name, value, si, ep );
		return;
	}
	throw Exception( EC.REFERENCE_ERROR, "Variable '" + name + "' is not defined", CONTEXT_GSE_CALL );
}

void Context::DestroyVariable( const std::string& name, CONTEXT_GSE_CALLABLE ) {
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		m_variables.erase( it );
		return;
	}
	throw Exception( EC.REFERENCE_ERROR, "Variable '" + name + "' is not defined", CONTEXT_GSE_CALL );
}

void Context::CreateBuiltin( const std::string& name, Value* const value, gse::ExecutionPointer& ep ) {
	CreateConst( "#" + name, value, {}, ep );
}

value::ValueRef* const Context::GetThis() const {
	return m_this;
}

value::ValueRef* const Context::GetParent() const {
	return m_parent;
}

void Context::Execute( const std::function< void() >& f ) {
	f();
}

ChildContext* const Context::ForkAndExecute(
	GSE_CALLABLE,
	const bool is_traceable,
	const std::function< void( ChildContext* const subctx ) >& f
) {
	CHECKACCUM( gc_space );
	NEWV( result, ChildContext, m_gse, this, si, is_traceable );
	// functions have access to parent variables
	for ( auto& it : m_ref_contexts ) {
		result->m_ref_contexts.insert_or_assign( it.first, it.second );
	}
	for ( auto& it : m_variables ) {
		result->m_ref_contexts.insert_or_assign( it.first, this );
	}
	if ( m_this ) {
		result->m_this = m_this;
	}
	if ( m_parent ) {
		result->m_parent = m_parent;
	}
	f( result );
	return result;
}

void Context::Clear() {
	m_variables.clear();
	for ( const auto& c : m_child_contexts ) {
		c->Clear();
	}
}

void Context::UnrefVariable( const std::string& name ) {
	const auto& it = m_ref_contexts.find( name );
	if ( it != m_ref_contexts.end() ) {
		m_ref_contexts.erase( it );
		for ( const auto& c : m_child_contexts ) {
			c->UnrefVariable( name );
		}
	}
}

void Context::GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) {
	gc::Object::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Context" );

	GC_DEBUG_BEGIN( "variables" );
	for ( const auto& v : m_variables ) {
		GC_DEBUG_BEGIN( v.first );
		GC_REACHABLE( v.second.value );
		GC_DEBUG_END();
	}
	GC_DEBUG_END();

	GC_DEBUG_BEGIN( "ref_contexts" );
	for ( const auto& it : m_ref_contexts ) {
		GC_REACHABLE( it.second );
	}
	GC_DEBUG_END();

/*	GC_DEBUG_BEGIN( "child_contexts" );
	for ( const auto& child : m_child_contexts ) {
		GC_REACHABLE( child );
	}
	GC_DEBUG_END();*/

	if ( m_this ) {
		GC_DEBUG_BEGIN( "this" );
		GC_REACHABLE( m_this );
		GC_DEBUG_END();
	}

	if ( m_parent ) {
		GC_DEBUG_BEGIN( "parent" );
		GC_REACHABLE( m_parent );
		GC_DEBUG_END();
	}

	GC_DEBUG_END();

}

void Context::CheckVariableName( const std::string& name, GSE_CALLABLE_NOGC ) {
	if ( s_variable_reserved_words.find( name ) != s_variable_reserved_words.end() ) {
		throw Exception( EC.INVALID_ASSIGNMENT, "'" + name + "' is reserved word", CONTEXT_GSE_CALL );
	}
}

void Context::AddChildContext( ChildContext* const child ) {
	ASSERT( m_child_contexts.find( child ) == m_child_contexts.end(), "child context already added" );
	m_child_contexts.insert( child );
}

void Context::RemoveChildContext( ChildContext* const child ) {
	if ( !m_child_contexts.empty() ) { // ?
		ASSERT( m_child_contexts.find( child ) != m_child_contexts.end(), "child context not found" );
		m_child_contexts.erase( child );
	}
}

void Context::SetThis( value::ValueRef* const _this ) {
	if ( m_this ) {
		m_parent = m_this;
	}
	m_this = _this;
}

}
}
