#pragma once

#include <unordered_set>

namespace gse {

namespace type {
class Object;
}

class Wrappable {
public:
	virtual ~Wrappable();
	void Link( type::Object* wrapobj );
	void Unlink( type::Object* wrapobj );
private:
	std::unordered_set< type::Object* > m_wrapobjs = {};
};

}
