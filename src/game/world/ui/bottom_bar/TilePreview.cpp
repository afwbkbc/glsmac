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
	
	/*NEW( m_mesh, mesh::Rectangle );
	NEW( m_preview, object::Mesh );
		m_preview->SetMesh( m_mesh );
		m_preview->SetTexture( g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", 1, 571, 56, 626 ) ); // tmp
	m_inner->AddChild( m_preview );*/
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
	
	Map::tile_vertices_t coords = tile->is_water_tile ? ts->layers[ Map::LAYER_WATER ].coords : ts->layers[ Map::LAYER_LAND ].coords;
	
	// absolute coords to relative
	#define x( _k ) coords._k -= coords.center
		x( left );
		x( top );
		x( right );
		x( bottom );
	#undef x
	coords.center = { 0.0f, 0.0f, 0.0f };
	
	NEW( m_mesh, mesh::Simple, 5, 4 );
	
	#define x( _k, _tx, _ty ) \
	auto _k = m_mesh->AddVertex( coords._k, { \
		_tx, \
		_ty \
	});
		x( center, 0.5f, 0.5f );
		x( left, 0.0f, 1.0f );
		x( top, 0.0f, 0.0f );
		x( right, 1.0f, 0.0f );
		x( bottom, 1.0f, 1.0f );
	#undef x

	#define x( _a, _b, _c ) m_mesh->AddSurface( { _a, _b, _c } )
		x( center, left, top );
		x( center, top, right );
		x( center, right, bottom );
		x( center, bottom, left );
	#undef x

	m_mesh->Finalize();
	
	NEW( m_preview, object::Mesh );
		m_preview->SetMesh( m_mesh );
		m_preview->SetTexture( g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", 1, 571, 56, 626 ) ); // tmp
	m_inner->AddChild( m_preview );

}

void TilePreview::HideTilePreview() {
	if ( m_preview ) {
		m_inner->RemoveChild( m_preview );
		m_preview = nullptr;
	}
}

}
}
}
