#pragma once

#include "UIContainer.h"

#include "Surface.h"

#include "ui/object/SoundEffect.h"

namespace ui {
namespace object {

CLASS( Popup, UIContainer )
	
	Popup( const std::string& class_name = "Popup" );

	void Create() override;
	void Destroy() override;

	// open or close popup
	void Open();
	void Close();
	
	// return true if ready to close, false if not yet (in that case you'll need to call ClosePopup manually when it's done)
	virtual bool MaybeClose();
	
protected:
	
	// sometimes it's needed to play sound at different time, i.e. when slidedown starts
	void PlayOpenSound();
	void PlayCloseSound();
	
private:
	struct {
		SoundEffect* open = nullptr;
		SoundEffect* close = nullptr;
	} m_sounds;
	
	// to ensure each sound is played only once
	struct {
		bool open = false;
		bool close = false;
	} m_sounds_played;
	
	bool m_is_opened = false;
	
	Surface* m_background_locker = nullptr;
	
};

}
}
