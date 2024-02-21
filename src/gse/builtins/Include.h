#pragma once

#include "gse/Bindings.h"

namespace gse {
namespace builtins {

class Include : public Bindings {
public:
	void AddToContext( gse::Context* ctx ) override;
};

}
}
