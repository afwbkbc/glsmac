#pragma once

#include "BBSection.h"

namespace types::texture {
class Texture;
}

namespace ui::object {
class Mesh;
class UIContainer;
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
	void SetTileYields( const std::vector< std::pair< std::string, size_t > >& yields );

private:

	struct preview_layer_t {
		::ui::object::Mesh* object;
		types::texture::Texture* texture;
	};
	std::vector< preview_layer_t > m_preview_layers = {}; // multiple layers of textures

	bool show_yields_page = false;
	struct {
		::ui::object::UIContainer* normal = nullptr;
		::ui::object::UIContainer* yields = nullptr;
	} m_pages = {};

	// TODO: multiline labels?
	struct {
		std::vector< ::ui::object::Label* > normal = {};
		std::vector< ::ui::object::Label* > yields = {};
		::ui::object::Label* footer = nullptr;
	} m_texts = {};

	void HideYields();

};

}
}
}
