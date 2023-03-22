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
	
#define SPRITE( _feature, _name, _texture, _y_shift ) \
	if ( tile->features & Tile::_feature ) { \
		GenerateSprite( tile, ts, _name, Map::s_consts.tc.ter1_pcx._texture, _y_shift ); \
	}
	
	if ( tile->is_water_tile ) {
		SPRITE( F_NUTRIENT_BONUS, "NutrientBonusSea", nutrient_bonus_sea[ m_map->GetRandom()->GetUInt( 0, 1 ) ], 0.12f );
		SPRITE( F_MINERALS_BONUS, "MineralsBonusSea", minerals_bonus_sea[ m_map->GetRandom()->GetUInt( 0, 1 ) ], 0.12f );
		SPRITE( F_ENERGY_BONUS, "EnergyBonusSea", energy_bonus_sea[ m_map->GetRandom()->GetUInt( 0, 1 ) ], 0.12f );
	}
	else {
		SPRITE( F_NUTRIENT_BONUS, "NutrientBonusLand", nutrient_bonus_land[ m_map->GetRandom()->GetUInt( 0, 1 ) ], 0.12f );
		SPRITE( F_MINERALS_BONUS, "MineralsBonusLand", minerals_bonus_land[ m_map->GetRandom()->GetUInt( 0, 1 ) ], 0.12f );
		SPRITE( F_ENERGY_BONUS, "EnergyBonusLand", energy_bonus_land[ m_map->GetRandom()->GetUInt( 0, 1 ) ], 0.12f );
	}
	SPRITE( F_MONOLITH, "Monolith", monolith[ 0 ], 0.0f );
	
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
