#pragma once

#include "UIContainer.h"

namespace ui {
namespace object {

class SoundEffect;

CLASS( Popup, UIContainer )

	Popup( const std::string& class_name = "Popup" );

	void Create() override;
	void Destroy() override;
	void ProcessEvent( event::UIEvent* event ) override;

	// open or close popup
	void Open();
	void Close();

	// return true if ready to close, false if not yet (in that case you'll need to call ClosePopup manually when it's done)
	virtual bool MaybeClose();

	// process events by other object(s) if not processed by popup
	void AddEventsTarget( UIObject* const object );
	void RemoveEventsTarget( UIObject* const object );

protected:

	// sometimes it's needed to play sound at different time, i.e. when slidedown starts
	void PlayOpenSound();
	void PlayCloseSound();

	// override if needed
	virtual void OnOpen() {};
	virtual void OnClose() {};

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

	std::unordered_set< UIObject* > m_events_targets = {};
};

}
}
