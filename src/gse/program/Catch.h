#pragma once

#include "Conditional.h"

namespace gse {
namespace program {

class Object;

class Catch : public Conditional {
public:

	Catch( const si_t& si, const Object* handlers );
	~Catch();

	const Object* handlers;

	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}

