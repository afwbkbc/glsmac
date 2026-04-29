#include "WrappedConfig.h"

#include "util/ConfigManager.h"

namespace config {
namespace wrapped {

Config::Config( const util::ConfigManager* const manager )
	: m_manager( manager ) {}

WRAPIMPL_BEGIN( Config )
	WRAPIMPL_PROPS};
	for ( const auto& it : m_manager->GetSettings() ) {
		properties.insert( { it.first, VALUE( gse::value::String, , it.second.second ) } );
	}
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Config )

}
}
