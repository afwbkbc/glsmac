#pragma once

#include <string>

#include "game/backend/base/Types.h"

namespace types::texture {
class Texture;
}

namespace game {
namespace frontend {
namespace base {

class FacilityDef {
public:
	FacilityDef(
		const std::string& name,
		const backend::base::render_info_t& render
	);

	const std::string m_name;

	types::texture::Texture* GetTexture() const;

private:
	types::texture::Texture* m_texture = nullptr;

};

}
}
}