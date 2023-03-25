#pragma once

#include "ui/object/Popup.h"

#include "Section.h"
#include "ui/object/Surface.h"
#include "ui/object/SoundEffect.h"

#include "util/Scroller.h"

namespace game {
namespace world {
	class World;
namespace ui {

CLASS( Popup, ::ui::object::Popup )
	
	Popup( World* world );
	
	void Create();
	void Iterate();
	void Destroy();
	
	void ProcessEvent( event::UIEvent* event );

	bool MaybeClose();
	
protected:
	World* m_world = nullptr;

private:
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
