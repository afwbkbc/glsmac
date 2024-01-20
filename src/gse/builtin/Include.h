#pragma once

#include "Builtin.h"

namespace gse {
namespace builtin {

class Include : public Builtin {
public:
	void AddToContext( gse::Context* ctx ) const override;
};

}
}
