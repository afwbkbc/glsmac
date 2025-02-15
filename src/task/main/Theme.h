#pragma once

#include "ui_legacy/theme/Theme.h"

#include "Style.h"

namespace task {
namespace intro {

CLASS( Theme, ::ui_legacy::theme::Theme )
	Theme();

protected:

private:
	Style m_style;
};

}
}
