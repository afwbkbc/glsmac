#pragma once

#include "ui/theme/Style.h"

namespace game {
namespace intro {

CHILD_CLASS( IntroLogo, ui::theme::Style )
protected:
	void SetStyle();

};

}
}