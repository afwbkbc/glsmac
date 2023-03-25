#pragma once

#include "ui/object/Popup.h"

#include "../Section.h"
#include "ui/object/Surface.h"
#include "ui/object/SoundEffect.h"

#include "util/Scroller.h"

namespace game {
namespace world {
	class World;
namespace ui {
namespace popup {

CLASS( Popup, ::ui::object::Popup )
	
	Popup( World* world );
	
	void Create();
	virtual void Align();
	void Iterate();
	void Destroy();
	void ProcessEvent( event::UIEvent* event );
	bool MaybeClose();
	void SetHeight( const coord_t px );
	
	void SetTitleText( const std::string& title_text );
	
protected:
	World* m_world = nullptr;

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
	
	struct {
		SoundEffect* slideup = nullptr;
		SoundEffect* slidedown = nullptr;
	} m_sounds;
};

}
}
}
}
