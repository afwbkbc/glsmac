#pragma once

#include "ui_legacy/object/Popup.h"

#include "util/Scroller.h"

namespace ui_legacy::object {
class Surface;
}

namespace game {
namespace frontend {

class Game;

namespace ui_legacy {

class Section;

namespace popup {

CLASS( Popup, ::ui_legacy::object::Popup )

	static constexpr size_t SLIDE_DURATION_MS = 100;

	Popup( Game* game );

	void Create() override;
	virtual void Align() override;
	void Iterate() override;
	void Destroy() override;
	void ProcessEvent( ::ui_legacy::event::UIEvent* event ) override;
	bool MaybeClose() override;
	void SetHeight( const coord_t px ) override;

	void SetTitleText( const std::string& title_text );

protected:
	Game* m_game = nullptr;

	void CloseNow();

	struct {
		bool no_outer_border = false;
		bool no_inner_border = false;
	} m_config = {};

private:
	coord_t m_original_height = 0; // TODO: move to styles?
	std::string m_title_text = "";

	Section* m_body = nullptr;
	struct {
		::ui_legacy::object::Surface* left_left = nullptr;
		::ui_legacy::object::Surface* left_right = nullptr;
		::ui_legacy::object::Surface* right_right = nullptr;
		::ui_legacy::object::Surface* right_left = nullptr;
	} m_side_frames;

	util::Scroller< coord_t > m_slide;
	bool m_is_closing = false;

};

}
}
}
}
