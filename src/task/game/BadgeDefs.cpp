#include "BadgeDefs.h"

#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "types/texture/Texture.h"
#include "InstancedSpriteManager.h"
#include "game/map/Consts.h"
#include "InstancedSprite.h"

namespace task {
namespace game {

const BadgeDefs::consts_t BadgeDefs::s_consts = {};

BadgeDefs::BadgeDefs( InstancedSpriteManager* ism )
	: m_ism( ism ) {

	auto* tl = g_engine->GetTextureLoader();

	// load badge sprites

	const uint8_t w = 23;
	const uint8_t h = 30;
	const uint8_t margin = 1;

	auto* texture = tl->LoadTexture( "flags.pcx" );

	const types::Color transparent( 0.0f, 0.0f, 0.0f, 0.0f );
	uint32_t x, y;
	for ( auto badge_type = BT_PROGENITOR ; badge_type <= BT_DEFAULT ; badge_type++ ) {
		for ( auto badge_mode = BT_NORMAL ; badge_mode <= BT_GREYEDOUT ; badge_mode++ ) {
			const uint8_t row = badge_type | badge_mode;
			auto& spritemap = m_unitbadge_sprites[ row ];
			for ( auto morale = ::game::unit::MORALE_MIN ; morale <= ::game::unit::MORALE_MAX ; morale++ ) {
				x = ( morale - ::game::unit::MORALE_MIN ) * ( w + margin ) + margin;
				y = row * ( h + margin ) + margin;

				// cut holes for healthbars
				texture->Fill( x + 6, y + 6, x + 7, y + 26, transparent );

				auto* sprite = m_ism->GetInstancedSprite(
					"Badge_" + std::to_string( badge_type ) + "_" + std::to_string( badge_mode ),
					texture,
					{
						x,
						y,
					},
					{
						w,
						h,
					},
					{
						x + w / 2,
						y + h / 2,
					},
					{
						::game::map::s_consts.tile.scale.x * s_consts.scale.x,
						::game::map::s_consts.tile.scale.y * s_consts.scale.y * ::game::map::s_consts.sprite.y_scale
					},
					0.55f
				);

				spritemap.insert(
					{
						morale,
						sprite
					}
				);
			}
		}
	}

	// load fake badge sprite

	m_fake_badge = m_ism->GetInstancedSprite(
		"Badge_FAKE",
		texture,
		{
			21,
			5,
		},
		{
			3,
			17,
		},
		{
			22,
			12,
		},
		{
			::game::map::s_consts.tile.scale.x * s_consts.fake_badges.scale.x,
			::game::map::s_consts.tile.scale.y * s_consts.fake_badges.scale.y * ::game::map::s_consts.sprite.y_scale
		},
		0.52f
	);

	// load healthbar sprites

	const auto res = s_consts.healthbars.resolution;

	const float stepval = 1.0f / ( res );
	types::Color black( 0.0f, 0.0f, 0.0f );
	types::Color color( 1.0f - stepval / 2, stepval / 2, 0.0f );

	m_healthbar_textures.reserve( res );
	m_healthbar_sprites.resize( res );
	for ( auto step = 0 ; step < res ; step++ ) {

		// generate healthbar texture
		texture = new types::texture::Texture( "HealthBar_" + std::to_string( step ), 1, res );

		texture->Fill( 0, 0, 0, step, color );
		if ( step < res - 1 ) {
			texture->Fill( 0, step + 1, 0, res - 1, black );
		}

		auto* sprite = m_ism->GetInstancedSprite(
			"Badge_Healthbar_" + std::to_string( step ),
			texture,
			{
				0,
				0,
			},
			{
				1,
				res,
			},
			{
				0,
				0,
			},
			{
				::game::map::s_consts.tile.scale.x * s_consts.healthbars.scale.x,
				::game::map::s_consts.tile.scale.y * s_consts.healthbars.scale.y * ::game::map::s_consts.sprite.y_scale
			},
			0.54f
		);

		m_healthbar_textures.push_back( texture );
		m_healthbar_sprites.at( step ).instanced_sprite = sprite;
		color.value.red -= stepval;
		color.value.green += stepval;
	}

}

BadgeDefs::~BadgeDefs() {
	for ( const auto& it : m_unitbadge_sprites ) {
		for ( const auto& it2 : it.second ) {
			m_ism->RemoveInstancedSpriteByKey( it2.second->key );
		}
	}
	m_ism->RemoveInstancedSpriteByKey( "Badge_FAKE [ 21 5 ] [ 3 17 ]" ); // TODO: refactor?
	const auto res = s_consts.healthbars.resolution;
	for ( auto step = 0 ; step < res ; step++ ) {
		m_ism->RemoveInstancedSpriteByKey( "Badge_Healthbar_" + std::to_string( step ) + " [ 0 0 ] [ 1 25 ]" ); // TODO: refactor
	}
	for ( const auto& texture : m_healthbar_textures ) {
		delete texture;
	}
}

const types::Vec3 BadgeDefs::GetBadgeCoords( const types::Vec3& unit_coords ) {
	return {
		unit_coords.x + ::game::map::s_consts.tile.scale.x * s_consts.offset.x,
		unit_coords.y - ::game::map::s_consts.tile.scale.y * s_consts.offset.y * ::game::map::s_consts.sprite.y_scale,
		unit_coords.z
	};
}

const types::Vec3 BadgeDefs::GetBadgeHealthbarCoords( const types::Vec3& unit_coords ) {
	return {
		unit_coords.x + ::game::map::s_consts.tile.scale.x * s_consts.healthbars.offset.x,
		unit_coords.y - ::game::map::s_consts.tile.scale.y * s_consts.healthbars.offset.y * ::game::map::s_consts.sprite.y_scale,
		unit_coords.z
	};
}

InstancedSprite* BadgeDefs::GetBadgeSprite( const badge_type_t badge_type, const ::game::unit::morale_t morale ) const {
	return m_unitbadge_sprites.at( badge_type ).at( morale );
}

InstancedSprite* BadgeDefs::GetFakeBadgeSprite() const {
	return m_fake_badge;
}

Sprite* BadgeDefs::GetBadgeHealthbarSprite( const float health ) {
	return &m_healthbar_sprites.at( round( health * ( s_consts.healthbars.resolution - 1 ) ) );
}

const size_t BadgeDefs::GetBadgeBlinkInterval() const {
	return s_consts.blink.interval_ms;
}

const types::Vec3 BadgeDefs::GetFakeBadgeCoords( const types::Vec3& coords, const uint8_t offset ) const {
	return {
		coords.x + ::game::map::s_consts.tile.scale.x * s_consts.fake_badges.offset.x + s_consts.fake_badges.step_x * offset,
		coords.y - ::game::map::s_consts.tile.scale.y * s_consts.fake_badges.offset.y * ::game::map::s_consts.sprite.y_scale,
		coords.z
	};
}

}
}
