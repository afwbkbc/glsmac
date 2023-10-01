#pragma once

#include "../UI.h"

#include "../../Types.h"

#include "ui/object/Surface.h"
#include "ui/object/Section.h"
#include "ui/object/Button.h"

#include "types/Texture.h"

// sections
#include "UnitPreview.h"
#include "TilePreview.h"
#include "MiddleArea.h"
#include "UnitsList.h"
#include "MiniMap.h"

// side menus
#include "left_menu/LeftMenu.h"

namespace task {
namespace game {

namespace ui {

CLASS( BottomBar, UI )

	BottomBar( Game* game )
		: UI( game, "BB" ) {}

	void Create() override;
	void Destroy() override;
	void Align() override;

	void PreviewTile( const tile_data_t& tile_data );
	void HideTilePreview();

	void SetMinimapTexture( types::Texture* texture );
	const Vec2< size_t > GetMinimapDimensions() const;
	void SetMinimapSelection( const Vec2< float > position_percents, const Vec2< float > zoom );
	const bool IsMouseDraggingMiniMap() const;

	void CloseMenus();

	const size_t GetMiddleHeight() const;

	void AddMessage( const std::string& text );
	void UpdateMapFileName();

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

	struct {
		UnitPreview* unit_preview = nullptr;
		TilePreview* tile_preview = nullptr;
		MiddleArea* middle_area = nullptr;
		UnitsList* units_list = nullptr;
		MiniMap* mini_map = nullptr;
	} m_sections = {};

	struct {
		menu::LeftMenu* left = nullptr;
	} m_side_menus = {};

	struct {
		types::Texture* minimap = nullptr;
	} m_textures = {};

	const UIEventHandler* m_mouse_blocker = nullptr;

};

}
}
}
