#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "base/Base.h"

#include "Value.h"
#include "type/Callable.h"

namespace gse {

CLASS( Context, base::Base )

	Context();
	~Context();

	const Value GetVariable( const std::string& name );
	void CreateVariable( const std::string& name, const Value& value );
	void UpdateVariable( const std::string& name, const Value& value );
	void PushScope();
	void PopScope();

	Context* const CreateFunctionScope(
		const std::vector< std::string > parameters,
		const type::Callable::function_arguments_t& arguments
	) const;

private:

	class Scope {
	public:
		typedef std::unordered_map< std::string, Value > variables_t;
		variables_t m_variables = {};

	};
	std::vector< Scope* > m_scopes = {};

};

}
