#include "Builtins.h"

#include "gse/type/Null.h"

namespace gse {
namespace builtin {

Value Builtins::Run( GSE* gse ) {

#ifdef DEBUG

	class GlobalTestMethod : public runnable::Native {
		Value Run( GSE* gse ) override {
			return Value( new type::Null() );
		}
	};
	NEWV( global_test_method, GlobalTestMethod );

	gse->RegisterGlobal( "_test", global_test_method );

#endif

	return Value( new type::Null() );
}

}
}