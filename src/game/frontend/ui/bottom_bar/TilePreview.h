#pragma once

#include "BBSection.h"

namespace types::texture {
class Texture;
}

namespace ui::object {
class Mesh;
class Label;
}

namespace game {
namespace frontend {

namespace tile {
class Tile;
}

namespace ui {

CLASS( TilePreview, BBSection )

	TilePreview( Game* game )
		: BBSection( game, "TilePreview" ) {}

	void Create() override;
	void Destroy() override;

	void PreviewTile( const tile::Tile* tile );
	void HideTilePreview();

private:

	struct preview_layer_t {
		::ui::object::Mesh* object;
		types::texture::Texture* texture;
	};
	std::vector< preview_layer_t > m_preview_layers = {}; // multiple layers of textures

	// TODO: multiline labels?
	std::vector< ::ui::object::Label* > m_info_lines = {};

};

}
}
}
