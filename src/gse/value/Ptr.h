#pragma once

#include "gse/Value.h"

#include <atomic>

namespace gse {
namespace value {

class Ptr : public Value {
public:

	static const value_type_t GetType() { return VT_PTR; }
	Ptr( gc::Space* const gc_space, const value_type_t ptr_type, void* const target );

	// TODO: is it possible to do operations without intermediate Value ?
	gse::Value* const GetValue() const;

private:
	const value_type_t m_ptr_type;
	void* const m_target;

};

}
}
