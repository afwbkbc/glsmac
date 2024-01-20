#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <vector>

#include "Context.h"

#include "Value.h"
#include "type/Callable.h"

namespace gse {

class GSE;

class ChildContext : public Context {
public:

	ChildContext( GSE* gse, Context& parent_context, const si_t& si, const bool is_traceable = true );

	const Context* GetParentContext() const override;
	const bool IsTraceable() const override;
	const std::string& GetSourceLine( const size_t line_num ) const override;
	const si_t& GetSI() const override;
	const script_info_t& GetScriptInfo() const override;

	void JoinContext() const;

private:
	Context& m_parent_context;

	const si_t m_si = {};
	const bool m_is_traceable;

};

}
