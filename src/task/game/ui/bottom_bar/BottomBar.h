#pragma once

#include "../UI.h"

#include "../../Types.h"

#include "ui/object/Surface.h"
#include "ui/object/Section.h"
#include "ui/object/Button.h"

#include "types/Texture.h"

// message label
#include "ui/object/Label.h"
#include "util/Timer.h"

// sections
#include "UnitPreview.h"
#include "TilePreview.h"
#include "MiddleArea.h"
#include "task/game/ui/bottom_bar/units_list/UnitsList.h"
#include "TurnCompleteButton.h"
#include "MiniMap.h"

// side menus
#include "left_menu/LeftMenu.h"
#include "right_menu/RightMenu.h"

namespace task {
namespace game {
class Game;
namespace ui {

CLASS( BottomBar, UI )

	BottomBar( Game* game )
		: UI( game, "BB" ) {}

	void Create() override;
	void Iterate() override;
	void Destroy() override;
	void Align() override;

	void PreviewTile( const tile_data_t& tile_data, const size_t selected_unit_id );
	void HideTilePreview();

	void PreviewUnit( const unit_data_t& unit_data );
	void HideUnitPreview();

	void SetMinimapTexture( types::Texture* texture );
	const Vec2< size_t > GetMinimapDimensions() const;
	void SetMinimapSelection( const Vec2< float > position_percents, const Vec2< float > zoom );
	const bool IsMouseDraggingMiniMap() const;

	void CloseMenus();

	const size_t GetMiddleHeight() const;

	void AddMessage( const std::string& text );
	void UpdateMapFileName();

	void SetTurnCompleteStatus( const bool is_turn_complete );

private:
	struct {
		Surface* left = nullptr;
		Surface* right = nullptr;
		Surface* middle = nullptr;
	} m_frames = {};

	std::vector< Surface* > m_backgrounds = {};

	struct {
		Button* menu = nullptr;
		Button* commlink = nullptr;
	} m_buttons = {};

	Label* m_message_label = nullptr;
	util::Timer m_message_label_clear_timer;

	struct {
		UnitPreview* unit_preview = nullptr;
		TilePreview* tile_preview = nullptr;
		MiddleArea* middle_area = nullptr;
		UnitsList* units_list = nullptr;
		TurnCompleteButton* turn_complete_button = nullptr;
		MiniMap* mini_map = nullptr;
	} m_sections = {};

	struct {
		menu::LeftMenu* left = nullptr;
		menu::RightMenu* right = nullptr;
	} m_side_menus = {};

	struct {
		types::Texture* minimap = nullptr;
	} m_textures = {};

	const UIEventHandler* m_mouse_blocker = nullptr;

};

}
}
}
