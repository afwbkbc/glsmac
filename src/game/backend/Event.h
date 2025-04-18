#pragma once

#include "gc/Object.h"

#include "gse/value/Callable.h"

namespace game {
namespace backend {

class Event : public gc::Object {
public:
	Event( gc::Space* const gc_space, const std::string& name, gse::value::Callable* const verify, gse::value::Callable* const apply, gse::value::Callable* const rollback );

	void GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) override;

	const std::string ToString() override;

	const std::string* const Verify( GSE_CALLABLE, const gse::value::function_arguments_t& args ) const;
	void Apply( GSE_CALLABLE, const gse::value::function_arguments_t& args ) const;
	void Rollback( GSE_CALLABLE, const gse::value::function_arguments_t& args ) const;

private:
	const std::string m_name;
	gse::value::Callable* m_verify;
	gse::value::Callable* m_apply;
	gse::value::Callable* m_rollback;

};

}
}
