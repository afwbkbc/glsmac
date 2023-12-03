#pragma once

#include "gse/callable/Native.h"

namespace gse {
namespace builtin {

class Builtins : public callable::Native {
	Value Run( GSE* gse, const Callable::function_arguments_t arguments ) override;
};

}
}
