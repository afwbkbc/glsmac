#include "ChildContext.h"

#include "gse/Exception.h"

namespace gse {
namespace context {

ChildContext::ChildContext( GSE* gse, Context* parent_context, Context* caller_context, const si_t& si, const bool is_traceable )
	: Context( gse )
	, m_parent_context( parent_context )
	, m_caller_context( caller_context )
	, m_si( si )
	, m_is_traceable( is_traceable ) {
	m_caller_context->IncRefs();
}

ChildContext::~ChildContext() {
	m_caller_context->DecRefs();
}

Context* ChildContext::GetParentContext() const {
	return m_parent_context;
}

Context* ChildContext::GetCallerContext() const {
	return m_caller_context;
}

const si_t& ChildContext::GetSI() const {
	return m_si;
}

const Context::script_info_t& ChildContext::GetScriptInfo() const {
	return m_parent_context->GetScriptInfo();
}

const bool ChildContext::IsTraceable() const {
	return m_is_traceable;
}

const std::string& ChildContext::GetSourceLine( const size_t line_num ) const {
	return m_parent_context->GetSourceLine( line_num );
}

void ChildContext::JoinContext() const {
	for ( const auto& it : m_variables ) {
		m_parent_context->SetVariable( it.first, it.second );
	}
}

}
}
