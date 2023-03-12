#pragma once

#include "ui/theme/Theme.h"

#include "Style.h"

namespace game {
namespace intro {

CLASS( Theme, ::ui::theme::Theme )
	Theme();

protected:
	
private:
	Style m_style;
};

}
}
