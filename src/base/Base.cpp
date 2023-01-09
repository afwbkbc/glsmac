#include "Base.h"

#include "engine/Engine.h"

namespace base {

void Base::Log( const std::string &text ) const {
	if ( g_engine != NULL )
		g_engine->GetLogger()->Log( "<" + GetName() + "> " + text );
}


} /* namespace base */
