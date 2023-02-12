#include <regex>

#include "TilePreview.h"

#include "engine/Engine.h"

namespace game {
namespace world {
namespace ui {

void TilePreview::Create() {
	UI::Create();
	
	NEW( m_outer, Section, "MapBottomBarSectionOuter" );
	AddChild( m_outer );
	
	NEW( m_inner, Section, "MapBottomBarSectionInner" );
	m_outer->AddChild( m_inner );
	
}

void TilePreview::Destroy() {
	HideTilePreview();
	m_outer->RemoveChild( m_inner );
	RemoveChild( m_outer );
	
	UI::Destroy();
}

void TilePreview::PreviewTile( const Map::tile_info_t& tile_info ) {
	HideTilePreview();
	
	auto tile = tile_info.tile;
	auto ts = tile_info.ts;
	auto ms = tile_info.ms;
	/*
	// TODO: copy actual coords
	Map::tile_vertices_t coords = tile->is_water_tile ? ts->layers[ Map::LAYER_WATER ].coords : ts->layers[ Map::LAYER_LAND ].coords;
	// absolute coords to relative
	#define x( _k ) coords._k -= coords.center
		x( left );
		x( top );
		x( right );
		x( bottom );
	#undef x
	coords.center = { 0.0f, 0.0f, 0.0f };
	*/
	Map::tile_vertices_t coords = {
		{ 0.0f, 0.0f, 1.0f }, // center
		{ -0.5f, 0.0f, 1.0f }, // left
		{ 0.0f, -0.5f, 1.0f }, // top
		{ 0.5f, 0.0f, 1.0f }, // right
		{ 0.0f, 0.5f, 1.0f }, // bottom
	};
	
	
	std::vector< map::Map::tile_layer_type_t > layers = {};
	
	if ( tile->is_water_tile ) {
		layers.push_back( map::Map::LAYER_WATER_SURFACE );
		layers.push_back( map::Map::LAYER_WATER );
	}
	else {
		layers.push_back( map::Map::LAYER_LAND );
	}
	
	for ( auto &lt : layers ) {
		
		NEWV( mesh, mesh::Render, 5, 4 );
	
		// copy texcoords from tile
		#define x( _k ) auto _k = mesh->AddVertex( coords._k, ts->layers[ lt ].tex_coords._k, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.5f, 0.7f } )
			x( center );
			x( left );
			x( top );
			x( right );
			x( bottom );
		#undef x

		#define x( _a, _b, _c ) mesh->AddSurface( { _a, _b, _c } )
			x( center, left, top );
			x( center, top, right );
			x( center, right, bottom );
			x( center, bottom, left );
		#undef x

		mesh->Finalize();

		NEWV( preview, object::Mesh, "MapBottomBarTilePreviewImage" );
			// TODO preview->SetAspectRatioMode( object::Mesh::AM_SCALE_HEIGHT );
			preview->SetMesh( mesh );
			preview->SetTexture( tile_info.ms->terrain_texture );
		m_previews.push_back( preview );
		m_inner->AddChild( preview );
	}
}

void TilePreview::HideTilePreview() {
	for ( auto& preview : m_previews ) {
		m_inner->RemoveChild( preview );
	}
	m_previews.clear();
}

}
}
}
