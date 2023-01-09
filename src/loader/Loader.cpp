#include "Loader.h"
#include "engine/Engine.h"
#include "config/Config.h"

namespace loader {

const string& Loader::GetRoot() {
	return g_engine->GetConfig()->GetGamePath();
}

}
