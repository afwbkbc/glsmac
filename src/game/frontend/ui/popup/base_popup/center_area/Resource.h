#pragma once

#include "CenterAreaPage.h"

namespace types::texture {
class Texture;
}

namespace ui::object {
class Mesh;
}

namespace game {
namespace frontend {

namespace base {
class Base;
}

namespace tile {
class Tile;
}

namespace ui {
namespace popup {
namespace base_popup {
namespace center_area {

CLASS( Resource, CenterAreaPage )

	Resource( CenterArea* center_area );

	void Create() override;
	void Destroy() override;

private:
	friend class CenterArea;
	void Update( base::Base* base );

	struct preview_layer_t {
		::ui::object::Mesh* object = nullptr;
		types::texture::Texture* texture = nullptr;
	};
	std::vector< preview_layer_t > m_preview_layers = {}; // multiple layers of textures

private:
	void PreviewTile( const tile::Tile* tile );
	void HideTilePreview();

};

}
}
}
}
}
}
