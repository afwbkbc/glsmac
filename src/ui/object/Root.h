#pragma once

#include "UIContainer.h"

namespace ui {
namespace object {

CLASS( Root, UIContainer )
	void Create() override;
	void Destroy() override;

};

}
}
