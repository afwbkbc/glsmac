#pragma once

#include "BottomBarBase.h"

#include "game/backend/turn/Types.h"

#include "types/Vec2.h"
#include "util/Timer.h"

namespace types::texture {
class Texture;
}

namespace ui_legacy::object {
class Button;
class Label;
}

namespace game {
namespace frontend {
class Game;
namespace tile {
class Tile;
}
class TileObject;

namespace ui_legacy {

class ObjectPreview;
class TilePreview;
class MiddleArea;
class StatusButton;
class ObjectsList;
class MiniMap;

namespace menu {
class LeftMenu;
class RightMenu;
}

CLASS( BottomBar, BottomBarBase )

	BottomBar( Game* game );

	void Create() override;
	void Iterate() override;
	void Destroy() override;

	void PreviewTile( tile::Tile* tile, const size_t selected_unit_id );
	void HideTilePreview();

	void SetTileYields( const std::vector< std::pair< std::string, size_t > >& yields ) const;

	void PreviewObject( const TileObject* object );
	void HideObjectPreview();

	void SetMinimapTexture( types::texture::Texture* texture );
	const types::Vec2< size_t > GetMinimapDimensions() const;
	void SetMinimapSelection( const types::Vec2< float > position_percents, const types::Vec2< float > zoom );
	const bool IsMouseDraggingMiniMap() const;

	void CloseMenus();

	const size_t GetMiddleHeight() const;

	void AddMessage( const std::string& text );
	void UpdateMapFileName();

	void SetTurnStatus( const backend::turn::turn_status_t status );

private:

	struct {
		::ui_legacy::object::Button* menu = nullptr;
		::ui_legacy::object::Button* commlink = nullptr;
	} m_buttons = {};

	::ui_legacy::object::Label* m_message_label = nullptr;
	util::Timer m_message_label_clear_timer;

	struct {
		ObjectPreview* unit_preview = nullptr;
		TilePreview* tile_preview = nullptr;
		MiddleArea* middle_area = nullptr;
		ObjectsList* objects_list = nullptr;
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

	const ::ui_legacy::event::UIEventHandler* m_mouse_blocker = nullptr;

};

}
}
}
