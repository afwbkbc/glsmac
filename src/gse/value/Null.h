#pragma once

namespace gse {
namespace value {

class Null : public Value {
public:
	Null()
		: Value( Value::T_NULL ) {}

	Null( const Null& other )
		: Null() {}
};

}
}
