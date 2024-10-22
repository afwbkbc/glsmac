#pragma once

#include "types/Buffer.h"

namespace game {
namespace backend {

class Game;

namespace base {

class Base;
class PopDef;

class Pop {
public:

	Pop( Base* base = nullptr, PopDef* def = nullptr );

	void Serialize( types::Buffer& buf ) const;
	void Unserialize( types::Buffer& buf, Game* game );

private:
	Base* m_base;
	PopDef* m_def;

};

}
}
}