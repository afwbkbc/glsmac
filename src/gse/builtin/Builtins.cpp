#include "Builtins.h"

namespace gse {
namespace builtin {

type::Type& Builtins::Run( GSE* gse ) {

#ifdef DEBUG

	class GlobalTestMethod : public runnable::Native {
		type::Type& Run( GSE* gse ) override {
			return nullptr;
		}
	};
	NEWV( global_test_method, GlobalTestMethod );

	gse->RegisterGlobal( "_test", global_test_method );

#endif

	return nullptr;
}

}
}