#include "BottomBar.h"

#include "engine/Engine.h"

#include "types/texture/Texture.h"
#include "ui/object/Surface.h"
#include "ui/object/Button.h"
#include "ui/object/Label.h"
#include "left_menu/LeftMenu.h"
#include "right_menu/RightMenu.h"
#include "UnitPreview.h"
#include "TilePreview.h"
#include "MiddleArea.h"
#include "units_list/UnitsList.h"
#include "MiniMap.h"
#include "StatusButton.h"
#include "ui/UI.h"
#include "task/game/Tile.h"
#include "graphics/Graphics.h"

namespace task {
namespace game {
namespace ui {

void BottomBar::Create() {
	UI::Create();

	// frames

	NEW( m_frames.left, ::ui::object::Surface, "BBFrameLeft" );
	m_frames.left->SetZIndex( 0.4f ); // needed because it needs to be above its background because in pcx there's transparent line around one of borders
	AddChild( m_frames.left );

	NEW( m_frames.middle, ::ui::object::Surface, "BBFrameMiddle" );
	AddChild( m_frames.middle );

	NEW( m_frames.right, ::ui::object::Surface, "BBFrameRight" );
	m_frames.right->SetZIndex( 0.3f ); // same as above (but needs to go under left panel if interface is shrunk)
	AddChild( m_frames.right );

	// background

	struct bg_coords_t {
		const ::ui::alignment_t alignment;
		const types::Vec2< size_t > position;
		const types::Vec2< size_t > size;
		const float zindex; // needed in case game window is shrunk to the point where frame starts to overlap other side
	};
	const std::vector< bg_coords_t > bg_coords = {
		{ ::ui::ALIGN_TOP | ::ui::ALIGN_LEFT,  { 4,   57 }, { 241, 196 }, 0.35f }, // left side part under unit and tile previews (+ a bit under units)
		{ ::ui::ALIGN_TOP | ::ui::ALIGN_RIGHT, { 4,   57 }, { 241, 196 }, 0.25f }, // part under minimap
		{ ::ui::ALIGN_BOTTOM,                  { 245, 5 },  { 0,   62 },  0.2f }, // bottom part (under units)
		{ ::ui::ALIGN_TOP,                     { 261, 67 }, { 0,   105 }, 0.2f }, // middle part (with info panels)
	};
	for ( auto& b : bg_coords ) {
		NEWV( background, ::ui::object::Surface, "BBFrameBackground" );
		background->SetAlign( b.alignment );
		if ( b.alignment & ::ui::ALIGN_LEFT ) {
			background->SetLeft( b.position.x );
		}
		else if ( b.alignment & ::ui::ALIGN_RIGHT ) {
			background->SetRight( b.position.x );
		}
		if ( b.alignment & ::ui::ALIGN_TOP ) {
			background->SetTop( b.position.y );
		}
		else if ( b.alignment & ::ui::ALIGN_BOTTOM ) {
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

	NEW( m_buttons.menu, ::ui::object::Button, "BBButtonMenu" );
	m_buttons.menu->SetLabel( "MENU" );
	m_buttons.menu->On(
		::ui::event::EV_BUTTON_CLICK, EH( this ) {
			if ( m_side_menus.left->IsVisible() ) {
				m_buttons.menu->RemoveStyleModifier( ::ui::M_SELECTED );
				m_side_menus.left->Hide();
			}
			else {
				m_buttons.menu->AddStyleModifier( ::ui::M_SELECTED );
				m_side_menus.left->Show();
			}
			return true;
		}
	);
	AddChild( m_buttons.menu );

	NEW( m_buttons.commlink, ::ui::object::Button, "BBButtonCommlink" );
	m_buttons.commlink->SetAlign( ::ui::ALIGN_TOP | ::ui::ALIGN_RIGHT );
	m_buttons.commlink->SetLabel( "COMMLINK" );
	m_buttons.commlink->On(
		::ui::event::EV_BUTTON_CLICK, EH( this ) {
			if ( m_side_menus.right->IsVisible() ) {
				m_buttons.commlink->RemoveStyleModifier( ::ui::M_SELECTED );
				m_side_menus.right->Hide();
			}
			else {
				m_buttons.commlink->AddStyleModifier( ::ui::M_SELECTED );
				m_side_menus.right->Show();
			}
			return true;
		}
	);
	AddChild( m_buttons.commlink );

	// message label

	NEW( m_message_label, ::ui::object::Label, "BBMessageLabel" );
	AddChild( m_message_label );

	// sections

	NEW( m_sections.unit_preview, UnitPreview, m_game );
	AddChild( m_sections.unit_preview );

	NEW( m_sections.tile_preview, TilePreview, m_game );
	AddChild( m_sections.tile_preview );

	NEW( m_sections.middle_area, MiddleArea, m_game );
	AddChild( m_sections.middle_area );

	NEW( m_sections.units_list, UnitsList, m_game, m_sections.unit_preview );
	AddChild( m_sections.units_list );

	NEW( m_sections.mini_map, MiniMap, m_game );
	if ( m_textures.minimap ) {
		m_sections.mini_map->SetMinimapTexture( m_textures.minimap );
	}
	AddChild( m_sections.mini_map );

	NEW( m_sections.status_button, StatusButton, m_game );
	m_sections.mini_map->AddChild( m_sections.status_button );

	// side menus
	auto* ui = g_engine->GetUI();

	// adding those to UI because they need to catch clicks outside of bottom bar
	NEW( m_side_menus.left, menu::LeftMenu, m_game );
	m_side_menus.left->SetBottom( GetHeight() - 1 ); // - 1 because there's 1 transparent pixel we need to cover
	ui->AddObject( m_side_menus.left );

	NEW( m_side_menus.right, menu::RightMenu, m_game );
	m_side_menus.right->SetBottom( GetHeight() - 2 ); // - 2 because there's 2 transparent pixels we need to cover
	ui->AddObject( m_side_menus.right );

	// other

	m_mouse_blocker = On(
		{
			::ui::event::EV_MOUSE_DOWN,
			::ui::event::EV_MOUSE_SCROLL
		}, EH( this ) {

			// don't let mouse events that weren't handled by child elements go through to map
			if (
				data->mouse.relative.x > 164 &&
					data->mouse.relative.x < g_engine->GetGraphics()->GetWindowWidth() - 162 &&
					data->mouse.relative.y <= 32
				) {
				// allow through transparent area at top
				return false;
			}

			// block everything else
			return true;
		}
	);
}

void BottomBar::Iterate() {
	UI::Iterate();

	if ( m_message_label_clear_timer.HasTicked() ) {
		m_message_label->SetText( "" );
	}
}

void BottomBar::Destroy() {

	m_message_label_clear_timer.Stop();

	Off( m_mouse_blocker );
	m_mouse_blocker = nullptr;

	RemoveChild( m_buttons.menu );
	RemoveChild( m_buttons.commlink );

	RemoveChild( m_message_label );

	for ( auto& b : m_backgrounds ) {
		RemoveChild( b );
	}

	RemoveChild( m_frames.left );
	RemoveChild( m_frames.middle );
	RemoveChild( m_frames.right );

	RemoveChild( m_sections.units_list );
	RemoveChild( m_sections.unit_preview );
	RemoveChild( m_sections.tile_preview );
	RemoveChild( m_sections.middle_area );
	m_sections.mini_map->RemoveChild( m_sections.status_button );
	RemoveChild( m_sections.mini_map );

	auto* ui = g_engine->GetUI();
	ui->RemoveObject( m_side_menus.left );
	ui->RemoveObject( m_side_menus.right );

	if ( m_textures.minimap ) {
		DELETE( m_textures.minimap );
		m_textures.minimap = nullptr;
	}

	UI::Destroy();
}

void BottomBar::Align() {
	UI::Align();

}

void BottomBar::PreviewTile( Tile* tile, const size_t selected_unit_id ) {
	m_sections.tile_preview->PreviewTile( tile );
	m_sections.units_list->ListUnits( tile->GetOrderedUnits(), selected_unit_id );
}

void BottomBar::HideTilePreview() {
	m_sections.tile_preview->HideTilePreview();
}

void BottomBar::PreviewUnit( const Unit* unit ) {
	m_sections.unit_preview->PreviewUnit( unit );
}

void BottomBar::HideUnitPreview() {
	m_sections.unit_preview->HideUnitPreview();
}

void BottomBar::SetMinimapTexture( types::texture::Texture* texture ) {
	if ( m_sections.mini_map ) {
		m_sections.mini_map->SetMinimapTexture( texture );
	}
	if ( m_textures.minimap ) {
		DELETE( m_textures.minimap );
	}
	m_textures.minimap = texture;
}

const types::Vec2< size_t > BottomBar::GetMinimapDimensions() const {
	ASSERT( m_sections.mini_map, "minimap not initialized" );
	return {
		224,
		112
	}; // TODO: get from minimap style
}

void BottomBar::SetMinimapSelection( const types::Vec2< float > position_percents, const types::Vec2< float > zoom ) {
	ASSERT( m_sections.mini_map, "minimap not initialized" );
	m_sections.mini_map->SetMinimapSelection( position_percents, zoom );
}

const bool BottomBar::IsMouseDraggingMiniMap() const {
	ASSERT( m_sections.mini_map, "minimap not initialized" );
	return m_sections.mini_map->IsMouseDragging();
}

void BottomBar::CloseMenus() {
	if ( m_buttons.menu->HasStyleModifier( ::ui::M_SELECTED ) ) {
		m_buttons.menu->RemoveStyleModifier( ::ui::M_SELECTED );
		m_side_menus.left->Hide();
	}
	if ( m_buttons.commlink->HasStyleModifier( ::ui::M_SELECTED ) ) {
		m_buttons.commlink->RemoveStyleModifier( ::ui::M_SELECTED );
		m_side_menus.right->Hide();
	}
}

const size_t BottomBar::GetMiddleHeight() const {
	return
		GetHeight() // element height
			- 32 // transparent area on top
			- 24 // transparent area below top line
		;
}

void BottomBar::AddMessage( const std::string& text ) {
	m_message_label->SetText( text );
	m_message_label_clear_timer.SetTimeout( 4000 );
	m_sections.middle_area->AddMessage( text );
}

void BottomBar::UpdateMapFileName() {
	m_sections.middle_area->UpdateMapFileName();
}

void BottomBar::SetTurnStatus( const ::game::turn::turn_status_t status ) {
	m_sections.status_button->SetStatus( status );
}

}
}
}
