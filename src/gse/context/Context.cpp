#include "Context.h"

#include "ChildContext.h"
#include "gse/GSE.h"
#include "gse/Exception.h"
#include "common/Common.h"
#include "gse/value/Callable.h"
#include "gse/value/Undefined.h"
#include "gc/Space.h"
#include "util/LogHelper.h"

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

void Context::SetVariable( const std::string& name, const var_info_t& var_info ) {
	THROW( "TODO: SetVariable" );
	m_variables.insert_or_assign( name, var_info );
}

void Context::CreateVariable( const std::string& name, Value* const value, CONTEXT_GSE_CALLABLE ) {
	ASSERT( value, "value is null" );
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

	GC_DEBUG_END();

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

}
}
