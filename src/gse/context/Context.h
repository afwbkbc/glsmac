#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <mutex>
#include <atomic>

#include "gc/Object.h"

#include "gse/Types.h"
#include "gse/type/Types.h"

#include "gse/Value.h"

namespace gse {

class GSE;

namespace type {
class Object;
}

namespace context {

class ChildContext;

class Context : public gc::Object {
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
	Context( GSE* gse );
	virtual ~Context();

	GSE* GetGSE() const;

	const bool HasVariable( const std::string& name );
	const Value GetVariable( const std::string& name, const si_t& si, gse::ExecutionPointer& ep );
	void SetVariable( const std::string& name, const var_info_t& var_info );
	void CreateVariable( const std::string& name, const Value& value, const si_t& si, gse::ExecutionPointer& ep );
	void CreateConst( const std::string& name, const Value& value, const si_t& si, gse::ExecutionPointer& ep );
	void UpdateVariable( const std::string& name, const Value& value, const si_t& si, gse::ExecutionPointer& ep );
	void DestroyVariable( const std::string& name, const si_t& si, gse::ExecutionPointer& ep );
	void CreateBuiltin( const std::string& name, const Value& value, gse::ExecutionPointer& ep );
	void PersistValue( const Value& value );
	void UnpersistValue( const Value& value );
	void UnpersistValue( const type::Type* type );

	void Execute( const std::function< void() >& f );

	ChildContext* const ForkAndExecute(
		GSE_CALLABLE,
		const bool is_traceable,
		const std::function< void( ChildContext* const subctx ) >& f
	);

	void Clear();

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

	std::unordered_map< const type::Type*, Value > m_persisted_values = {};

	void CollectActiveObjects( std::unordered_set< Object* >& active_objects ) override;

	std::atomic< bool > m_is_executing = false;

private:

	std::mutex m_gc_mutex;
	std::unordered_set< ChildContext* > m_child_contexts = {};
	std::unordered_set< type::Type* > m_child_objects = {};

private:
	friend class ChildContext;
	void AddChildContext( ChildContext* const child );
	void RemoveChildContext( ChildContext* const child );

private:
	friend class type::Object;
	void AddChildObject( type::Type* const child );
	void RemoveChildObject( type::Type* const child );

};

}
}
