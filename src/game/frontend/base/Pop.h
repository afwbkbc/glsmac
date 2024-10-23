#pragma once

#include <cstdint>

namespace types::texture {
class Texture;
}

namespace game {
namespace frontend {
namespace base {

class Base;
class PopDef;

class Pop {
public:
	Pop( const Base* base, const PopDef* def, const uint8_t variant );

	types::texture::Texture* GetTexture() const;

	const Base* m_base;
	const PopDef* m_def;
	uint8_t m_variant;
};

}
}
}
