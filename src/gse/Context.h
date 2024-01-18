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

	Context( const Context* parent_context, const source_lines_t& source_lines, const si_t& si );
	~Context();

	const Value GetVariable( const std::string& name, const si_t* si );
	void CreateVariable( const std::string& name, const Value& value, const si_t* si );
	void CreateConst( const std::string& name, const Value& value, const si_t* si );
	void UpdateVariable( const std::string& name, const Value& value, const si_t* si );
	const Context* GetParentContext() const;

	const si_t& GetSI() const;

	void AddSourceLine( const std::string& source_line );
	void AddSourceLines( const source_lines_t& source_lines );
	const std::string GetSourceLine( const size_t line_num ) const;
	const source_lines_t& GetSourceLines() const;

	Context* const ForkContext(
		const si_t& call_si,
		const std::vector< std::string > parameters = {},
		const type::Callable::function_arguments_t& arguments = {}
	);
	void JoinContext( Context* const other ) const;

private:

	const Context* m_parent_context;
	source_lines_t m_source_lines;

	const si_t m_si = {};

	struct var_info_t {
		Value value;
		bool is_const;
	};
	typedef std::unordered_map< std::string, var_info_t > variables_t;
	variables_t m_variables = {};
	typedef std::unordered_map< std::string, Context* > ref_contexts_t;
	ref_contexts_t m_ref_contexts = {};

};

}
