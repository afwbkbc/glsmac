#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "Types.h"
#include "type/Types.h"

#include "Value.h"

namespace gse {

class GSE;

class ChildContext;

class Context {
protected:
	struct var_info_t {
		Value value;
		bool is_const;
	};
	struct script_info_t {
		const std::string path;
		const std::string file;
		const std::string directory;
	};

public:
	Context( GSE* gse )
		: m_gse( gse ) {}
	virtual ~Context() = default;

	GSE* GetGSE() const;

	void IncRefs();
	void DecRefs();

	const bool HasVariable( const std::string& name );
	const Value GetVariable( const std::string& name, const si_t* si );
	void SetVariable( const std::string& name, const var_info_t& var_info );
	void CreateVariable( const std::string& name, const Value& value, const si_t* si );
	void CreateConst( const std::string& name, const Value& value, const si_t* si );
	void UpdateVariable( const std::string& name, const Value& value, const si_t* si );
	void CreateBuiltin( const std::string& name, const Value& value );
	void PersistValue( const Value& value );
	void UnpersistValue( const Value& value );
	void UnpersistValue( const type::Type* type );

	ChildContext* const ForkContext(
		Context* caller_context,
		const si_t& call_si,
		const bool is_traceable,
		const std::vector< std::string > parameters = {},
		const type::function_arguments_t& arguments = {}
	);

	virtual Context* GetParentContext() const = 0;
	virtual Context* GetCallerContext() const = 0;
	virtual const bool IsTraceable() const = 0;
	virtual const std::string& GetSourceLine( const size_t line_num ) const = 0;
	virtual const si_t& GetSI() const = 0;
	virtual const script_info_t& GetScriptInfo() const = 0;

protected:
	GSE* m_gse;
	size_t m_refs = 0;

	typedef std::unordered_map< std::string, var_info_t > variables_t;
	variables_t m_variables = {};
	typedef std::unordered_map< std::string, Context* > ref_contexts_t;
	ref_contexts_t m_ref_contexts = {};

	std::unordered_map< const type::Type*, Value > m_persisted_values = {};
};

}
