#pragma once

namespace gse {
namespace value {

class Nothing : public Value {
public:

	static const type_t GetType() { return Value::T_NOTHING; }

	Nothing()
		: Value( GetType() ) {}

	Nothing( const Nothing& other )
		: Nothing() {}
};

}
}
