#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <shared_mutex>
#include <atomic>

#include "gc/Object.h"

#include "gse/Types.h"
#include "gse/value/Types.h"

#include "gse/Value.h"

#define CONTEXT_GSE_CALLABLE const si_t& si, gse::ExecutionPointer& ep
#define CONTEXT_GSE_CALL this, si, ep

namespace gse {

class GSE;

namespace value {
class Object;
class ValueRef;
}

namespace context {

class ChildContext;

class Context : public gc::Object {
protected:
	struct var_info_t {
		Value* value;
		bool is_const;
	};
	struct script_info_t {
		const std::string path;
		const std::string file;
		const std::string directory;
	};

public:
	Context( GSE* gse );
	virtual ~Context();

	GSE* GetGSE() const;

	const bool HasVariable( const std::string& name );
	Value* const GetVariable( const std::string& name, CONTEXT_GSE_CALLABLE );
	void CreateVariable( const std::string& name, Value* const value, CONTEXT_GSE_CALLABLE );
	void CreateConst( const std::string& name, Value* const value, CONTEXT_GSE_CALLABLE );
	void UpdateVariable( const std::string& name, Value* const value, CONTEXT_GSE_CALLABLE );
	void DestroyVariable( const std::string& name, CONTEXT_GSE_CALLABLE );
	void CreateBuiltin( const std::string& name, Value* const value, gse::ExecutionPointer& ep );

	value::ValueRef* const GetThis() const;
	value::ValueRef* const GetParent() const;

	void Execute( const std::function< void() >& f );

	ChildContext* const ForkAndExecute(
		GSE_CALLABLE,
		const bool is_traceable,
		const std::function< void( ChildContext* const subctx ) >& f
	);

	void Clear();
	void UnrefVariable( const std::string& name );

	virtual void GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) override;

	virtual Context* GetParentContext() const = 0;
	virtual const bool IsTraceable() const = 0;
	virtual const std::string& GetSourceLine( const size_t line_num ) const = 0;
	virtual const si_t& GetSI() const = 0;
	virtual const script_info_t& GetScriptInfo() const = 0;

protected:

	GSE* m_gse;

	typedef std::unordered_map< std::string, var_info_t > variables_t;
	variables_t m_variables = {};
	typedef std::unordered_map< std::string, Context* > ref_contexts_t;
	ref_contexts_t m_ref_contexts = {};

private:
	std::unordered_set< ChildContext* > m_child_contexts = {};
	std::unordered_set< Value* > m_child_objects = {};

	value::ValueRef* m_this = nullptr;
	value::ValueRef* m_parent = nullptr;

	void CheckVariableName( const std::string& name, GSE_CALLABLE_NOGC );

private:
	friend class ChildContext;
	void AddChildContext( ChildContext* const child );
	void RemoveChildContext( ChildContext* const child );

private:
	friend class value::Object;
	void SetThis( value::ValueRef* const _this );

};

}
}
