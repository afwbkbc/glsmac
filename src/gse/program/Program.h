#pragma once

#include <string>

namespace gse {
namespace program {

class Scope;

class Program {
public:

	Program( const Scope* body, const bool is_body_owned );
	~Program();

	const Scope* body;

	const std::string Dump() const;

private:
	const bool m_is_body_owned;
};

}
}
