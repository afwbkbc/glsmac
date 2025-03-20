#include "ChildContext.h"

#include "gse/Exception.h"

namespace gse {
namespace context {

ChildContext::ChildContext( GSE* gse, Context* parent_context, const si_t& si, const bool is_traceable )
	: Context( gse )
	, m_parent_context( parent_context )
	, m_si( si )
	, m_is_traceable( is_traceable ) {
	ASSERT_NOLOG( m_parent_context, "parent context not set" );
	ASSERT_NOLOG( m_parent_context->GetParentContext() != this, "circular context dependency" );
	m_parent_context->AddChildContext( this );
}

ChildContext::~ChildContext() {
	if ( m_parent_context ) {
		m_parent_context->RemoveChildContext( this );
	}
}

Context* ChildContext::GetParentContext() const {
	return m_parent_context;
}

const si_t& ChildContext::GetSI() const {
	return m_si;
}

const Context::script_info_t& ChildContext::GetScriptInfo() const {
	if ( m_parent_context ) {
		return m_parent_context->GetScriptInfo();
	}
	else {
		static const Context::script_info_t s_empty_script_info = {
			"",
			"",
			""
		};
		return s_empty_script_info;
	}
}

const bool ChildContext::IsTraceable() const {
	return m_is_traceable;
}

const std::string& ChildContext::GetSourceLine( const size_t line_num ) const {
	if ( m_parent_context ) {
		return m_parent_context->GetSourceLine( line_num );
	}
	else {
		static const std::string s_empty = "";
		return s_empty;
	}
}

void ChildContext::Detach() {
	ASSERT_NOLOG( m_parent_context, "detach but parent context not set" );
	m_parent_context = nullptr;
}

void ChildContext::JoinContext() const {
	for ( const auto& it : m_variables ) {
		m_parent_context->SetVariable( it.first, it.second );
	}
}

void ChildContext::GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) {
	Context::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "ChildContext" );

	if ( m_parent_context ) { // could have been detached
		GC_DEBUG_BEGIN( "parent_context" );
		GC_REACHABLE( m_parent_context );
		GC_DEBUG_END();
	}

	GC_DEBUG_END();
}

}
}
