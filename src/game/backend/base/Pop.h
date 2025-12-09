#pragma once

#include "types/Buffer.h"

#include "gse/Wrappable.h"

namespace game {
namespace backend {

class Game;

namespace base {

class Base;
class PopDef;

class Pop : public gse::Wrappable {
public:

	Pop( Base* base = nullptr, PopDef* def = nullptr, const uint8_t variant = 0 );

	void Serialize( types::Buffer& buf ) const;
	void Deserialize( types::Buffer& buf, Game* game );

	void SetBase( Base* const base );

	WRAPDEFS_PTR( Pop );

	Base* m_base;
	PopDef* m_def;
	uint8_t m_variant = 0;

};

}
}
}