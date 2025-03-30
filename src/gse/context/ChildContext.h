#pragma once

#include <string>

#include "Context.h"

namespace gse {

class GSE;

namespace context {

class ChildContext : public Context {
public:

	ChildContext( GSE* gse, Context* parent_context, const si_t& si, const bool is_traceable = true );
	~ChildContext();

	Context* GetParentContext() const override;
	virtual const bool IsTraceable() const override;
	const std::string& GetSourceLine( const size_t line_num ) const override;
	const si_t& GetSI() const override;
	const script_info_t& GetScriptInfo() const override;

	void Detach();

	void JoinContext();

	void GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) override;

private:
	Context* m_parent_context = nullptr; // scope parent

	const si_t m_si = {};
	const bool m_is_traceable;

};

}
}
