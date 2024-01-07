#pragma once

#include "gse/callable/Native.h"

namespace gse {
namespace builtin {

class Console : public callable::Native {
	Value Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) override;
};

}
}
