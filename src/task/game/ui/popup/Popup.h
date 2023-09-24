#pragma once

#include "ui/object/Popup.h"

#include "../Section.h"
#include "ui/object/Surface.h"

#include "util/Scroller.h"

namespace task {
namespace game {
class Game;
namespace ui {
namespace popup {

CLASS( Popup, ::ui::object::Popup )

	Popup( Game* game );

	void Create() override;
	virtual void Align() override;
	void Iterate() override;
	void Destroy() override;
	void ProcessEvent( event::UIEvent* event ) override;
	bool MaybeClose() override;
	void SetHeight( const coord_t px ) override;

	void SetTitleText( const std::string& title_text );

protected:
	Game* m_game = nullptr;

	void CloseNow();

private:
	coord_t m_original_height = 0; // TODO: move to styles?
	std::string m_title_text = "";

	Section* m_body = nullptr;
	struct {
		::ui::object::Surface* left_left = nullptr;
		::ui::object::Surface* left_right = nullptr;
		::ui::object::Surface* right_right = nullptr;
		::ui::object::Surface* right_left = nullptr;
	} m_side_frames;

	util::Scroller< coord_t > m_slide;
	bool m_is_closing = false;

};

}
}
}
}
