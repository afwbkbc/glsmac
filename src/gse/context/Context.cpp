#include "Context.h"

#include "ChildContext.h"
#include "gse/GSE.h"
#include "gse/Exception.h"
#include "common/Common.h"
#include "gse/value/Callable.h"
#include "gse/value/Undefined.h"
#include "gc/Space.h"

namespace gse {
namespace context {

Context::Context( gse::GSE* gse )
	: m_gse( gse ) {
}

Context::~Context() {
	std::lock_guard< std::mutex > guard( m_gc_mutex );

	for ( const auto& child_context : m_child_contexts ) {
		child_context->Detach();
	}
}

GSE* Context::GetGSE() const {
	return m_gse;
}

const bool Context::HasVariable( const std::string& name ) {
	std::lock_guard< std::mutex > guard( m_gc_mutex );
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
	m_gc_mutex.lock();
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		const auto result = it->second.value;
		m_gc_mutex.unlock();
		return result;
	}
	const auto ref_it = m_ref_contexts.find( name );
	if ( ref_it != m_ref_contexts.end() ) {
		const auto ref = ref_it->second;
		m_gc_mutex.unlock();
		ASSERT_NOLOG( ref != this, "unexpected ref context recursion (was this context freed while in use?)" );
		return ref->GetVariable( name, si, ep );
	}
	m_gc_mutex.unlock();
	throw Exception( EC.REFERENCE_ERROR, "Variable '" + name + "' is not defined", CONTEXT_GSE_CALL );
}

void Context::SetVariable( const std::string& name, const var_info_t& var_info ) {
	std::lock_guard< std::mutex > guard( m_gc_mutex );
	m_variables.insert_or_assign( name, var_info );
}

void Context::CreateVariable( const std::string& name, Value* const value, CONTEXT_GSE_CALLABLE ) {
	std::lock_guard< std::mutex > guard( m_gc_mutex );
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
	std::lock_guard< std::mutex > guard( m_gc_mutex );
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
	std::lock_guard< std::mutex > guard( m_gc_mutex );
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
	std::lock_guard< std::mutex > guard( m_gc_mutex );
	const auto it = m_variables.find( name );
	if ( it != m_variables.end() ) {
		m_variables.erase( it );
		return;
	}
	throw Exception( EC.REFERENCE_ERROR, "Variable '" + name + "' is not defined", CONTEXT_GSE_CALL );
}

void Context::CreateBuiltin( const std::string& name, Value* const value, gc::Space* const gc_space, gse::ExecutionPointer& ep ) {
	CreateConst( "#" + name, value, {}, ep );
}

void Context::PersistValue( Value* const value ) {
	std::lock_guard< std::mutex > guard( m_gc_mutex );
	ASSERT_NOLOG( m_persisted_values.find( value ) == m_persisted_values.end(), "value already persisted" );
	m_persisted_values.insert(
		{
			value,
			value
		}
	);
}

void Context::UnpersistValue( Value* const value ) {
	std::lock_guard< std::mutex > guard( m_gc_mutex );
	ASSERT_NOLOG( m_persisted_values.find( value ) != m_persisted_values.end(), "value was not persisted" );
	m_persisted_values.erase( value );
}

void Context::Execute( const std::function< void() >& f ) {
	ASSERT_NOLOG( !m_is_executing, "context already executing" );
	m_is_executing = true;
	f();
	ASSERT_NOLOG( m_is_executing, "context not executing" );
	m_is_executing = false;
}

ChildContext* const Context::ForkAndExecute(
	GSE_CALLABLE,
	const bool is_traceable,
	const std::function< void( ChildContext* const subctx ) >& f
) {
	NEWV( result, ChildContext, m_gse, this, si, is_traceable );
	ASSERT_NOLOG( !result->m_is_executing, "context already executing" );
	result->m_is_executing = true;
	m_gse->GetGCSpace()->Add( result );
	// functions have access to parent variables
	for ( auto& it : m_ref_contexts ) {
		result->m_ref_contexts.insert_or_assign( it.first, it.second );
	}
	for ( auto& it : m_variables ) {
		result->m_ref_contexts.insert_or_assign( it.first, this );
	}
	try {
		f( result );
	}
	catch ( const std::exception& e ) {
		result->m_is_executing = false;
		throw;
	};
	ASSERT_NOLOG( result->m_is_executing, "context not executing" );
	result->m_is_executing = false;
	return result;
}

void Context::Clear() {
	m_gc_mutex.lock();
	const auto children = m_child_contexts;
	const auto persisted_values = m_persisted_values;
	m_variables.clear();
	m_gc_mutex.unlock();
	for ( const auto& c : children ) {
		c->Clear();
	}
	for ( const auto& value : persisted_values ) {
		UnpersistValue( value.second );
	}
}

void Context::CollectActiveObjects( std::unordered_set< Object* >& active_objects ) {
	std::lock_guard< std::mutex > guard( m_gc_mutex );

	if ( !m_is_executing &&
		m_persisted_values.empty() &&
		m_child_contexts.empty()
		) {
		// context is not used for anything anymore
		return;
	}

	active_objects.insert( this );
	for ( const auto& child_context : m_child_contexts ) {
		child_context->CollectActiveObjects( active_objects );
	}
}

void Context::AddChildContext( ChildContext* const child ) {
	std::lock_guard< std::mutex > guard( m_gc_mutex );
	ASSERT_NOLOG( m_child_contexts.find( child ) == m_child_contexts.end(), "child context already added" );
	m_child_contexts.insert( child );
}

void Context::RemoveChildContext( ChildContext* const child ) {
	std::lock_guard< std::mutex > guard( m_gc_mutex );
	if ( !m_child_contexts.empty() ) { // ?
		ASSERT_NOLOG( m_child_contexts.find( child ) != m_child_contexts.end(), "child context not found" );
		m_child_contexts.erase( child );
	}
}

void Context::AddChildObject( Value* const child ) {
	std::lock_guard< std::mutex > guard( m_gc_mutex );
	ASSERT_NOLOG( m_child_objects.find( child ) == m_child_objects.end(), "child context already added" );
	m_child_objects.insert( child );
}

void Context::RemoveChildObject( Value* const child ) {
	std::lock_guard< std::mutex > guard( m_gc_mutex );
	if ( !m_child_objects.empty() ) { // ?
		ASSERT_NOLOG( m_child_objects.find( child ) != m_child_objects.end(), "child context not found" );
		m_child_objects.erase( child );
	}
}

}
}
