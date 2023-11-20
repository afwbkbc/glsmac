#pragma once

#include "gse/runnable/Native.h"

namespace gse {
namespace builtin {

class Console : public runnable::Native {
	void Run() override;
};

}
}
