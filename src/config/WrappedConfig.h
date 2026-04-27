#pragma once

#include "gse/Wrappable.h"

namespace util {
class ConfigManager;
}

namespace config {
namespace wrapped {

class Config : public gse::Wrappable {
public:

	Config( const util::ConfigManager* const manager );

	WRAPDEFS_PTR( Config )

private:
	const util::ConfigManager* const m_manager;

};

}
}
