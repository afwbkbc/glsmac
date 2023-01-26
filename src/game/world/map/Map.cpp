#include "Map.h"

#include "engine/Engine.h"

#include "scene/mesh/Mesh.h"

namespace game {
namespace world {
namespace map {

#define MAP_POSITION { 0.0, 0.0, 0.0 }
#define MAP_ROTATION { 0.0, 0.0, 0.0 }

#define TILE_WIDTH 1.0f
#define TILE_HEIGHT 1.0f
#define TILE_Z_SCALE 1.0f

#define TILE_HALFWIDTH ( TILE_WIDTH / 2 )
#define TILE_HALFHEIGHT ( TILE_HEIGHT / 2 )

Map::Map( Scene* scene )
	: m_scene( scene )
{
	
	// tmp for testing
	//m_textures.push_back( g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", 1, 571, 56, 626 ) ); // frame
	m_textures.push_back( g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", 394, 687, 449, 742 ) ); // fungus
}

Map::~Map() {
	if ( m_terrain_actor ) {
		DELETE( m_terrain_actor );
	}
	if ( m_tiles ) {
		DELETE( m_tiles );
	}
}

void Map::SetTiles( Tiles* tiles ) {
	ASSERT( !m_tiles, "map tiles already set" );
	m_tiles = tiles;
	GenerateActor();
}

actor::Mesh* Map::GetActor() const {
	return m_terrain_actor;
}

void Map::GenerateActor() {
	ASSERT( m_tiles, "map tiles not set" );
	
	if ( m_terrain_actor ) {
		m_scene->RemoveActor( m_terrain_actor );
		DELETE( m_terrain_actor );
		Log( "Regenerating terrain actor" );
	}
	else {
		Log( "Generating terrain actor" );
	}
	
	size_t w = m_tiles->GetWidth();
	size_t h = m_tiles->GetHeight();
	
	Tile* tile;
	mesh::Mesh::index_t center, left, right, top, bottom;
	float xpos, ypos;
	
	NEWV( mesh, mesh::Mesh, w * h * 5, w * h * 4 );
	
	float ox = -( (float) TILE_WIDTH * m_tiles->GetWidth() / 2 - TILE_HALFWIDTH / 2 );
	float oy = - ( (float) TILE_HEIGHT * m_tiles->GetHeight() / 4 - TILE_HALFHEIGHT );
	
	for ( size_t y = 0 ; y < h ; y++ ) {
		for ( size_t x = 0 ; x < w ; x++ ) {
			tile = m_tiles->At( x, y );
			
			xpos = ox + TILE_WIDTH * x;
			ypos = oy + TILE_HEIGHT * y / 2;
			if ( y % 2 ) {
				xpos += TILE_HALFWIDTH;
			}
			
			// TODO: clamper?
#define SCALE_Z( _var ) ( TILE_Z_SCALE * (float) ( Tile::ELEVATION_MIN + _var ) / ( Tile::ELEVATION_MAX - Tile::ELEVATION_MIN ) )
			
			// tmp
			Vec2< float > tx[4];
			switch ( rand() % 4 ) {
				case 0: {
					tx[0] = { 0, 0 };
					tx[1] = { 1, 0 };
					tx[2] = { 1, 1 };
					tx[3] = { 0, 1 };
					break;
				}
				case 1: {
					tx[0] = { 1, 0 };
					tx[1] = { 1, 1 };
					tx[2] = { 0, 1 };
					tx[3] = { 0, 0 };
					break;
				}
				case 2: {
					tx[0] = { 1, 1 };
					tx[1] = { 0, 1 };
					tx[2] = { 0, 0 };
					tx[3] = { 1, 0 };
					break;
				case 3: {
					tx[0] = { 0, 1 };
					tx[1] = { 0, 0 };
					tx[2] = { 1, 0 };
					tx[3] = { 1, 1 };
					break;
				}
				}
			}
			
			left = mesh->AddVertex( { xpos - TILE_HALFWIDTH, ypos, SCALE_Z( *tile->elevation.left ) }, tx[0] );
			top = mesh->AddVertex( { xpos, ypos - TILE_HALFHEIGHT, SCALE_Z( *tile->elevation.top ) },  tx[1] );
			right = mesh->AddVertex( { xpos + TILE_HALFWIDTH, ypos, SCALE_Z( *tile->elevation.right ) },  tx[2] );
			bottom = mesh->AddVertex( { xpos, ypos + TILE_HALFHEIGHT, SCALE_Z( tile->elevation.bottom ) },  tx[3] );
			
			center = mesh->AddVertex( { xpos, ypos, SCALE_Z( tile->elevation.center ) }, { 0.5, 0.5 } );
			
			mesh->AddSurface( { center, left, top } );
			mesh->AddSurface( { center, top, right } );
			mesh->AddSurface( { center, right, bottom } );
			mesh->AddSurface( { center, bottom, left } );

		}
	}
	
	mesh->Finalize();
	
	NEW( m_terrain_actor, actor::Mesh, "MapTerrain", mesh );
		m_terrain_actor->SetTexture( m_textures[0] );
		m_terrain_actor->SetPosition( MAP_POSITION );
		// center at cursor
/*		m_terrain_actor->SetPosition( {
			- ( (float) TILE_WIDTH * m_tiles->GetWidth() / 2 - TILE_HALFWIDTH / 2 ),
			( (float) TILE_HEIGHT * m_tiles->GetHeight() / 4 - TILE_HALFHEIGHT ),
			0.0f
		});*/
		m_terrain_actor->SetAngle( MAP_ROTATION );
	m_scene->AddActor( m_terrain_actor );
}

}
}
}
