#include "TileResources.h"

#include <vector>

#include "ui/UI.h"
#include "ui/dom/Widget.h"
#include "scene/actor/Mesh.h"
#include "game/frontend/Game.h"
#include "game/backend/map/tile/Tile.h"
#include "game/frontend/sprite/InstancedSprite.h"
#include "scene/actor/Instanced.h"
#include "scene/actor/Sprite.h"
#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/Player.h"
#include "game/backend/resource/ResourceManager.h"
#include "game/frontend/resource/ResourceManager.h"

namespace game {
namespace frontend {
namespace widget {

TileResources::TileResources( Game* const game, ui::UI* const ui )
	: Widget(
	game, ui, ui::WT_TILE_RESOURCES, "tile-resources", {
		{ "tile", { true, gse::VT_OBJECT, ::game::backend::map::tile::Tile::WRAP_CLASS } },
	}
) {}

void TileResources::Register( ui::dom::Widget* const widget ) {
	widget->OnUpdate(
		F_WIDGET_UPDATE( this ) {
			if ( data ) {
				auto* const t = ::game::backend::map::tile::Tile::Unwrap( data->value.at( "tile" ) );
				ASSERT( t, "invalid tile ptr" );
				Update( widget, t );
			}
		}
	);
}

void TileResources::Update( ui::dom::Widget* const widget, const void* const data ) {
	widget->Clear();

	auto* const tile = (::game::backend::map::tile::Tile*)data;

	auto* const backend = m_game->GetBackend();

	::game::backend::map::tile::Tile::resources_t resources = {};
	backend->GetState()->WithGSE(
		backend, [ &resources, &tile, &backend ]( GSE_CALLABLE ) {
			resources = tile->GetResources( GSE_CALL, backend->GetPlayer()->GetSlot() );
		}, nullptr, true
	);

	size_t count = 0;
	auto* const rm = m_game->GetRM();
	for ( const auto& it : resources ) {
		if ( it.second > 0 ) {
			count++;
		}
	}

	const auto x_scale = 0.4f;
	const auto y_scale = 0.8f;

	types::Vec2< size_t > max_wh = { 0, 0 };

	for ( const auto& id : backend->GetRM()->GetDefinedResourcesOrder() ) {
		const auto& r_max_wh = rm->GetResourceMaxWH( id );
		if ( r_max_wh.x > max_wh.x ) {
			max_wh.x = r_max_wh.x;
		}
		if ( r_max_wh.y > max_wh.y ) {
			max_wh.y = r_max_wh.y;
		}
	}

	if ( count > 0 ) {
		std::vector< const types::mesh::Render* > meshes = {};
		std::vector< types::texture::Texture* > textures = {};
		std::vector< backend::map::pcx_texture_coordinates_t > whs = {};
		size_t index = 0;
		size_t total_width = 0;
		size_t max_height = 0;
		size_t total_possible_width = 0;
		for ( const auto& id : backend->GetRM()->GetDefinedResourcesOrder() ) {
			total_possible_width += max_wh.x;
			const auto& resource_it = resources.find( id );
			if ( resource_it == resources.end() ) {
				continue;
			}
			const auto& value = resource_it->second;
			if ( value == 0 ) {
				continue;
			}
			auto* instanced_sprite = rm->GetResourceSprite( id, value );
			ASSERT( instanced_sprite, "resource sprite is null" );
			auto* actor = instanced_sprite->actor;
			auto sprite = actor->GetSpriteActor();
			const auto& wh = instanced_sprite->wh;
			if ( wh.y > max_height ) {
				max_height = wh.y;
			}
			total_width += wh.x;
			meshes.push_back( sprite->GenerateMesh() );
			textures.push_back( sprite->GetTexture() );
			whs.push_back( wh );
		}
		ASSERT( meshes.size() == count, "meshes size mismatch" );
		ASSERT( textures.size() == count, "textures size mismatch" );
		ASSERT( whs.size() == count, "whs size mismatch" );
		ASSERT( max_wh.x > 0, "max_wh x is zero" );
		ASSERT( max_wh.y > 0, "max_wh y is zero" );
		auto offset = 0.5f - (float)total_width / total_possible_width / 2;
		for ( size_t i = 0 ; i < count ; i++ ) {
			const auto& wh = whs.at( i );
			const auto w = (float)wh.x / max_wh.x * x_scale;
			const auto h = (float)wh.y / max_wh.y * y_scale;
			AddMeshAndTexture(
				widget,
				index++,
				meshes.at( i ),
				textures.at( i ),
				false,
				{
					w,
					h,
				},
				{
					offset,
					0.5f - h / 2,
				}
			);
			offset += w;
		}
	}
	else {
		auto* instanced_sprite = rm->GetNoResourceSprite();
		ASSERT( instanced_sprite, "no-resource sprite is null" );
		auto* actor = instanced_sprite->actor;
		auto sprite = actor->GetSpriteActor();
		const auto& wh = instanced_sprite->wh;
		const auto w = (float)wh.x / max_wh.x * x_scale;
		const auto h = (float)wh.y / max_wh.y * y_scale;
		AddMeshAndTexture(
			widget,
			0,
			sprite->GenerateMesh(),
			sprite->GetTexture(),
			false,
			{
				w,
				h,
			},
			{
				0.5f - w / 2,
				0.5f - h / 2,
			}
		);
	}

}

}
}
}
