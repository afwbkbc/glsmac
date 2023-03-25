#pragma once

#include "UIContainer.h"

#include "Surface.h"

namespace ui {
namespace object {

CLASS( Popup, UIContainer )
	
	Popup( const std::string& class_name = "Popup" );

	void Create();
	void Destroy();

	// return true if ready to close, false if not yet (in that case you'll need to call ClosePopup manually when it's done)
	virtual bool MaybeClose();
	
private:
	Surface* m_background_locker = nullptr;
	
};

}
}
