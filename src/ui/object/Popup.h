#pragma once

#include "Section.h"

#include "Surface.h"

namespace ui {
namespace object {

CLASS( Popup, Section )
	
	Popup( const std::string& class_name = "Popup" );

	void Create();
	void Destroy();

private:
	Surface* m_background_locker = nullptr;
	
};

}
}
