#pragma once

#include "task/game/ui/UI.h"

#include "game/backend/turn/Types.h"

#include "types/Vec2.h"
#include "util/Timer.h"

namespace types::texture {
class Texture;
}

namespace ui::object {
class Surface;
class Button;
class Label;
}

namespace task {
namespace game {
class Game;
namespace tile {
class Tile;
}
namespace unit {
class Unit;
}
namespace ui {

class UnitPreview;
class TilePreview;
class MiddleArea;
class StatusButton;
class UnitsList;
class MiniMap;

namespace menu {
class LeftMenu;
class RightMenu;
}

CLASS( BottomBar, UI )

	BottomBar( Game* game )
		: UI( game, "BB" ) {}

	void Create() override;
	void Iterate() override;
	void Destroy() override;
	void Align() override;

	void PreviewTile( tile::Tile* tile, const size_t selected_unit_id );
	void HideTilePreview();

	void PreviewUnit( const unit::Unit* unit );
	void HideUnitPreview();

	void SetMinimapTexture( types::texture::Texture* texture );
	const types::Vec2< size_t > GetMinimapDimensions() const;
	void SetMinimapSelection( const types::Vec2< float > position_percents, const types::Vec2< float > zoom );
	const bool IsMouseDraggingMiniMap() const;

	void CloseMenus();

	const size_t GetMiddleHeight() const;

	void AddMessage( const std::string& text );
	void UpdateMapFileName();

	void SetTurnStatus( const ::game::backend::turn::turn_status_t status );

private:
	struct {
		::ui::object::Surface* left = nullptr;
		::ui::object::Surface* right = nullptr;
		::ui::object::Surface* middle = nullptr;
	} m_frames = {};

	std::vector< ::ui::object::Surface* > m_backgrounds = {};

	struct {
		::ui::object::Button* menu = nullptr;
		::ui::object::Button* commlink = nullptr;
	} m_buttons = {};

	::ui::object::Label* m_message_label = nullptr;
	util::Timer m_message_label_clear_timer;

	struct {
		UnitPreview* unit_preview = nullptr;
		TilePreview* tile_preview = nullptr;
		MiddleArea* middle_area = nullptr;
		UnitsList* units_list = nullptr;
		StatusButton* status_button = nullptr;
		MiniMap* mini_map = nullptr;
	} m_sections = {};

	struct {
		menu::LeftMenu* left = nullptr;
		menu::RightMenu* right = nullptr;
	} m_side_menus = {};

	struct {
		types::texture::Texture* minimap = nullptr;
	} m_textures = {};

	const ::ui::event::UIEventHandler* m_mouse_blocker = nullptr;

};

}
}
}
