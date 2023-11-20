#pragma once

#include "gse/runnable/Native.h"

namespace gse {
namespace builtin {

class Builtins : public runnable::Native {
	void Run() override;
};

}
}
