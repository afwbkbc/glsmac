#pragma once

#include "gse/runnable/Native.h"

namespace gse {
namespace builtin {

class Console : public runnable::Native {
	type::Type& Run( GSE* gse ) override;
};

}
}
