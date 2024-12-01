#include "Root.h"

namespace ui {
namespace object {

void Root::Create() {
	UIContainer::Create();
	EnableActors();
}

void Root::Destroy() {
	DisableActors();
	UIContainer::Destroy();
}

}
}
