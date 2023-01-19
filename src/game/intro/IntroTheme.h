#pragma once

#include "ui/theme/Theme.h"

#include "IntroStyle.h"

namespace game {
namespace intro {

CHILD_CLASS( IntroTheme, ui::theme::Theme )
	IntroTheme();

protected:
	
private:
	IntroStyle m_style;
};

}
}
