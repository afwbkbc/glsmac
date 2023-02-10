#include "TileSelection.h"

#include "engine/Engine.h"

namespace game {
namespace world {
namespace actor {

TileSelection::TileSelection( map::Map::tile_vertices_t coords )
	: Actor( "TileSelection" )
{
	NEWV( mesh, types::mesh::Render, 5, 4 );
	
		#define x( _k, _tx, _ty ) \
		coords._k.z += 0.01f; /* prevent obstruction by tile */ \
		auto _k = mesh->AddVertex( coords._k, { \
			_tx, \
			_ty \
		});
			x( center, 0.5f, 0.5f );
			x( left, 0.0f, 1.0f );
			x( top, 0.0f, 0.0f );
			x( right, 1.0f, 0.0f );
			x( bottom, 1.0f, 1.0f );
		#undef x

		#define x( _a, _b, _c ) mesh->AddSurface( { _a, _b, _c } )
			x( center, left, top );
			x( center, top, right );
			x( center, right, bottom );
			x( center, bottom, left );
		#undef x

	mesh->Finalize();
	
	SetMesh( mesh );
	SetTexture( g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", 1, 571, 56, 626 ) );
}

}
}
}
