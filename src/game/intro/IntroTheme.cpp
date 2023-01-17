#include "IntroTheme.h"

#include "IntroLogo.h"

namespace game {
namespace intro {

IntroTheme::IntroTheme() : ui::theme::Theme() {
	NEWV( style, IntroLogo );
	AddStyle( style );
}

}
}
