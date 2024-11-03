#pragma once

#include <unordered_set>

#include "gse/Value.h"

namespace gse {

namespace type {
class Object;
}

class Wrappable {
public:
	virtual ~Wrappable();
	virtual const gse::Value Wrap( const bool dynamic = false ) = 0;
	void Link( type::Object* wrapobj );
	void Unlink( type::Object* wrapobj );
private:
	std::unordered_set< type::Object* > m_wrapobjs = {};
};

}
