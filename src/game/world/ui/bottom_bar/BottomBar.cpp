#include "BottomBar.h"

#include "engine/Engine.h"

namespace game {
namespace world {
namespace ui {

void BottomBar::Create() {
	UI::Create();
	
	// frames
	
	NEW( m_frames.left, Surface, "MapBottomBarFrameLeft" );
		m_frames.left->SetZIndex( 0.4f ); // needed because it needs to be above its background because in pcx there's transparent line around one of borders
	AddChild( m_frames.left );
	
	NEW( m_frames.middle, Surface, "MapBottomBarFrameMiddle" );
	AddChild( m_frames.middle );
	
	NEW( m_frames.right, Surface, "MapBottomBarFrameRight" );
		m_frames.right->SetZIndex( 0.3f ); // same as above (but needs to go under left panel if interface is shrunk)
	AddChild( m_frames.right );
	
	// background
	
	struct bg_coords_t {
		const UIObject::alignment_t alignment;
		const Vec2< size_t > position;
		const Vec2< size_t > size;
		const float zindex; // needed in case game window is shrunk to the point where frame starts to overlap other side
	};
	const std::vector< bg_coords_t > bg_coords = {
		{ UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT, { 4, 57 }, { 241, 196 }, 0.35f }, // left side part under unit and tile previews (+ a bit under units)
		{ UIObject::ALIGN_TOP | UIObject::ALIGN_RIGHT, { 4, 57 }, { 241, 196 }, 0.25f }, // part under minimap
		{ UIObject::ALIGN_BOTTOM, { 245, 5 }, { 0, 62 }, 0.2f }, // bottom part (under units)
		{ UIObject::ALIGN_TOP, { 261, 67 }, { 0, 105 }, 0.2f }, // middle part (with info panels)
	};
	for ( auto& b : bg_coords ) {
		NEWV( background, Surface, "MapBottomBarFrameBackground" );
			background->SetAlign( b.alignment );
			if ( b.alignment & UIObject::ALIGN_LEFT ) {
				background->SetLeft( b.position.x );
			}
			else if ( b.alignment & UIObject::ALIGN_RIGHT ) {
				background->SetRight( b.position.x );
			}
			if ( b.alignment & UIObject::ALIGN_TOP ) {
				background->SetTop( b.position.y );
			}
			else if ( b.alignment & UIObject::ALIGN_BOTTOM ) {
				background->SetBottom( b.position.y );
			}
			if ( b.size.x ) {
				background->SetWidth( b.size.x );
			}
			else {
				background->SetLeft( b.position.x );
				background->SetRight( b.position.x );
			}
			background->SetHeight( b.size.y );
			background->SetZIndex( b.zindex );
		m_backgrounds.push_back( background );
		AddChild( background );
	}
	
	// buttons
	
	NEW( m_buttons.menu, Button, "MapBottomBarButtonMenu" );
		m_buttons.menu->SetLabel( "MENU" );
	AddChild( m_buttons.menu );
	
	NEW( m_buttons.commlink, Button, "MapBottomBarButtonCommlink" );
		m_buttons.commlink->SetAlign( UIObject::ALIGN_TOP | UIObject::ALIGN_RIGHT );
		m_buttons.commlink->SetLabel( "COMMLINK" );
	AddChild( m_buttons.commlink );
	
	// sections
	
	NEW( m_sections.unit_preview, UnitPreview, m_world );
	AddChild( m_sections.unit_preview );
	
	NEW( m_sections.tile_preview, TilePreview, m_world );
	AddChild( m_sections.tile_preview );
	
	NEW( m_sections.info_panels, InfoPanels, m_world );
	AddChild( m_sections.info_panels );
	
	NEW( m_sections.units_list, UnitsList, m_world );
	AddChild( m_sections.units_list );
	
	NEW( m_sections.mini_map, MiniMap, m_world );
		if ( m_textures.minimap ) {
			m_sections.mini_map->SetMinimapTexture( m_textures.minimap );
		}
	AddChild( m_sections.mini_map );
	
	m_mouse_blocker = On( UIEvent::EV_MOUSE_DOWN, EH( this ) {
		// don't let clicks that weren't handled by child elements go through to map
		if (
			data->mouse.relative.x > 164 &&
			data->mouse.relative.x < g_engine->GetGraphics()->GetViewportWidth() - 162 &&
			data->mouse.relative.y <= 32
		) {
			// allow clicks through transparent area at top
			return false;
		}
		
		// block all other clicks
		return true;
	});
}

void BottomBar::Destroy() {
	
	Off( m_mouse_blocker );
	m_mouse_blocker = nullptr;
	
	RemoveChild( m_buttons.menu );
	RemoveChild( m_buttons.commlink );
	
	for ( auto& b : m_backgrounds ) {
		RemoveChild( b );
	}
	
	RemoveChild( m_frames.left );
	RemoveChild( m_frames.middle );
	RemoveChild( m_frames.right );
	
	RemoveChild( m_sections.unit_preview );
	RemoveChild( m_sections.tile_preview );
	RemoveChild( m_sections.info_panels );
	RemoveChild( m_sections.mini_map );
	RemoveChild( m_sections.units_list);
	
	if ( m_textures.minimap ) {
		DELETE( m_textures.minimap );
		m_textures.minimap = nullptr;
	}
	
	UI::Destroy();
}

void BottomBar::Align() {
	UI::Align();
	
	
}

void BottomBar::PreviewTile( const Map* map, const Map::tile_info_t& tile_info ) {
	m_sections.tile_preview->PreviewTile( map, tile_info );
}

void BottomBar::HideTilePreview() {
	m_sections.tile_preview->HideTilePreview();
}

void BottomBar::SetMinimapTexture( types::Texture* texture ) {
	if ( m_sections.mini_map ) {
		m_sections.mini_map->SetMinimapTexture( texture );
	}
	if ( m_textures.minimap ) {
		DELETE( m_textures.minimap );
	}
	m_textures.minimap = texture;
}

}
}
}
