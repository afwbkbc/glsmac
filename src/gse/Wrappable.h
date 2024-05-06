#pragma once

#include "type/Object.h"

namespace gse {

class Wrappable {
public:
	virtual ~Wrappable();
	void Link( type::Object* wrapobj );
	void Unlink( type::Object* wrapobj );
private:
	std::unordered_set< type::Object* > m_wrapobjs = {};
};

}
