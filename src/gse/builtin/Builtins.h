#pragma once

#include "gse/runnable/Native.h"

namespace gse {
namespace builtin {

class Builtins : public runnable::Native {
	Value Run( GSE* gse ) override;
};

}
}
