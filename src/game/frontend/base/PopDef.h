#pragma once

#include <string>

#include "game/backend/base/Types.h"

namespace types::texture {
class Texture;
}

namespace game {
namespace frontend {
namespace base {

class PopDef {
public:
	PopDef(
		const std::string& name,
		const backend::base::pop_render_infos_t& renders_human,
		const backend::base::pop_render_infos_t& renders_progenitor
	);

	const std::string m_name;

	types::texture::Texture* GetTexture( const uint8_t variant, const bool is_progenitor ) const;

private:
	std::vector< types::texture::Texture* > m_textures_human = {};
	std::vector< types::texture::Texture* > m_textures_progenitor = {};

};

}
}
}