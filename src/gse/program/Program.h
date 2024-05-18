#pragma once

#include <string>

namespace gse {
namespace program {

class Scope;

class Program {
public:

	Program( const Scope* body );
	~Program();

	const Scope* body;

	const std::string Dump() const;

};

}
}
