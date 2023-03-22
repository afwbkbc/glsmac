#include "Sprites.h"

#include "scene/actor/Sprite.h"

namespace game {
namespace world {
namespace map {

void Sprites::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	// TODO: keep existing ones?
	for ( auto& sprite : ts->sprites ) {
		m_map->GetScene()->RemoveActor( sprite.actor );
		DELETE( sprite.actor );
	}
	ts->sprites.clear();
	
#define SPRITE( _name, _texture, _y_shift ) \
	GenerateSprite( tile, ts, _name, Map::s_consts.tc.ter1_pcx._texture, _y_shift )
	
#define FEATURE_SPRITE( _feature, _name, _texture, _y_shift ) \
	if ( tile->features & Tile::_feature ) { \
		SPRITE( _name, _texture, _y_shift ); \
	}
	
	if ( tile->is_water_tile ) {
		switch ( tile->bonus ) {
			case Tile::B_NUTRIENT: {
				SPRITE( "NutrientBonusSea", nutrient_bonus_sea[ m_map->GetRandom()->GetUInt( 0, 1 ) ], 0.12f );
				break;
			}
			case Tile::B_ENERGY: {
				SPRITE( "EnergyBonusSea", energy_bonus_sea[ m_map->GetRandom()->GetUInt( 0, 1 ) ], 0.12f );
				break;
			}
			case Tile::B_MINERALS: {
				SPRITE( "MineralsBonusSea", minerals_bonus_sea[ m_map->GetRandom()->GetUInt( 0, 1 ) ], 0.12f );
				break;
			}
			default: {
				// nothing
			}
		}
		FEATURE_SPRITE( F_GEOTHERMAL, "Geothermal", geothermal[ 0 ], 0.0f );
		FEATURE_SPRITE( F_UNITY_POD, "UnityPodSea", unity_pod_sea[ m_map->GetRandom()->GetUInt( 0, 2 ) ], 0.0f );
	}
	else {
		switch ( tile->bonus ) {
			case Tile::B_NUTRIENT: {
				SPRITE( "NutrientBonusLand", nutrient_bonus_land[ m_map->GetRandom()->GetUInt( 0, 1 ) ], 0.12f );
				break;
			}
			case Tile::B_ENERGY: {
				SPRITE( "EnergyBonusLand", energy_bonus_land[ m_map->GetRandom()->GetUInt( 0, 1 ) ], 0.12f );
				break;
			}
			case Tile::B_MINERALS: {
				SPRITE( "MineralsBonusLand", minerals_bonus_land[ m_map->GetRandom()->GetUInt( 0, 1 ) ], 0.12f );
				break;
			}
			default: {
				// nothing
			}
		}
		FEATURE_SPRITE( F_URANIUM, "Uranium", uranium[ 0 ], 0.0f );
		FEATURE_SPRITE( F_UNITY_POD, "UnityPodLand", unity_pod_land[ m_map->GetRandom()->GetUInt( 0, 2 ) ], 0.0f );
	}
	
	FEATURE_SPRITE( F_MONOLITH, "Monolith", monolith[ 0 ], 0.0f );

#undef FEATURE_SPRITE
#undef SPRITE
	
}

void Sprites::GenerateSprite( const Tile* tile, Map::tile_state_t* ts, const std::string& name, const Map::pcx_texture_coordinates_t& tex_coords, const float y_shift ) {
	Map::tile_state_t::sprite_t sprite = {};
	
	sprite.tex_coords = tex_coords;
	sprite.actor = m_map->GenerateTerrainSpriteActor( name, sprite.tex_coords );
	
	const auto& coords = tile->is_water_tile ? ts->layers[ Map::LAYER_WATER ].coords : ts->layers[ Map::LAYER_LAND ].coords;
	
	sprite.actor->SetPosition({
		coords.center.x,
		- ( coords.center.y + y_shift ), // TODO: fix y inversion
		coords.center.z
	});
	
	m_map->GetScene()->AddActor( sprite.actor );
	ts->sprites.push_back( sprite );
}

}
}
}
