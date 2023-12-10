#pragma once

#include <string>

#include "Operand.h"

namespace gse {
namespace program {

class Nothing : public Operand {
public:

	Nothing()
		: Operand( OT_NOTHING ) {}

};

}
}

