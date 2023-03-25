#pragma once

#include "UIContainer.h"

#include "Surface.h"

namespace ui {
namespace object {

CLASS( Popup, UIContainer )
	
	Popup( const std::string& class_name = "Popup" );

	void Create();
	void Destroy();

	// open or close popup
	void Open();
	void Close();
	
	// return true if ready to close, false if not yet (in that case you'll need to call ClosePopup manually when it's done)
	virtual bool MaybeClose();
	
private:
	bool m_is_opened = false;
	
	Surface* m_background_locker = nullptr;
	
};

}
}
