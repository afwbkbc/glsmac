#pragma once

#include "BBSection.h"

namespace types::texture {
class Texture;
}

namespace ui_legacy::object {
class Mesh;
class UIContainer;
class Label;
}

namespace game {
namespace frontend {

namespace tile {
class Tile;
}

namespace ui_legacy {

CLASS( TilePreview, BBSection )

	TilePreview( Game* game )
		: BBSection( game, "TilePreview" ) {}

	void Create() override;
	void Destroy() override;

	void PreviewTile( const tile::Tile* tile );
	void HideTilePreview();

private:

	struct preview_layer_t {
		::ui_legacy::object::Mesh* object;
		types::texture::Texture* texture;
	};
	std::vector< preview_layer_t > m_preview_layers = {}; // multiple layers of textures

	bool show_yields_page = false;
	struct {
		::ui_legacy::object::UIContainer* normal = nullptr;
		::ui_legacy::object::UIContainer* yields = nullptr;
	} m_pages = {};

	// TODO: multiline labels?
	struct {
		std::vector< ::ui_legacy::object::Label* > normal = {};
		std::vector< ::ui_legacy::object::Label* > yields = {};
		::ui_legacy::object::Label* footer = nullptr;
	} m_texts = {};

	void HideYields();

};

}
}
}
