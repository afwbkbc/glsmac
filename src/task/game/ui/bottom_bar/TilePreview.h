#pragma once

#include "BBSection.h"

#include "../../Types.h"

#include "ui/object/Mesh.h"
#include "ui/object/Label.h"

namespace task {
namespace game {
namespace ui {

CLASS( TilePreview, BBSection )

	TilePreview( Game* game ) : BBSection( game, "TilePreview" ) {}
	
	void Create() override;
	void Destroy() override;

	void PreviewTile( const tile_data_t& tile_data );
	void HideTilePreview();
	
private:

	struct preview_layer_t {
		object::Mesh* object;
		types::Texture* texture;
	};
	std::vector< preview_layer_t > m_preview_layers = {}; // multiple layers of textures
	
	// TODO: multiline labels?
	std::vector< Label* > m_info_lines = {};
	
};
	
}
}
}
