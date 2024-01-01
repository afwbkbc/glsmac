#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <vector>

#include "base/Base.h"

#include "Value.h"
#include "type/Callable.h"

namespace gse {

CLASS( Context, base::Base )

	typedef std::vector< std::string > source_lines_t;

	Context( const Context* parent_context, const source_lines_t* source_lines );
	~Context();

	const Value GetVariable( const std::string& name );
	void CreateVariable( const std::string& name, const Value& value );
	void UpdateVariable( const std::string& name, const Value& value );
	void PushScope();
	void PopScope();
	const size_t GetScopeDepth() const;
	const Context* GetParentContext() const;
	void SetSI( const si_t& si );
	const si_t& GetSI() const;

	const std::string GetSourceLine( const size_t line_num ) const;

	Context* const CreateFunctionScope(
		const std::string& function_name,
		const std::vector< std::string > parameters,
		const type::Callable::function_arguments_t& arguments
	) const;

private:

	const Context* m_parent_context;
	const source_lines_t* m_source_lines;

	si_t m_si = {};

	class Scope {
	public:
		typedef std::unordered_map< std::string, Value > variables_t;
		variables_t m_variables = {};

	};
	std::vector< Scope* > m_scopes = {};

};

}
