#include "Console.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/value/Undefined.h"

#include "logger/Stdout.h"
#include "util/LogHelper.h"

namespace gse {
namespace builtins {

#if defined( DEBUG ) || defined( FASTDEBUG )
static bool s_is_capturing = false;
static std::string s_capture_buffer = "";
void Console::CaptureStart() const {
	ASSERT( !s_is_capturing, "already capturing" );
	s_capture_buffer.clear();
	s_is_capturing = true;
}
const std::string& Console::CaptureStopGet() const {
	ASSERT( s_is_capturing, "not capturing" );
	s_is_capturing = false;
	return s_capture_buffer;
}
#endif

void Console::AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) {

	ctx->CreateBuiltin( "print", NATIVE_CALL() {
		std::string line = "";
		for ( const auto& it : arguments ) {
			if ( !line.empty() ) {
				line += " ";
			}
			line += it->ToString();
		}
#if defined( DEBUG ) || defined( FASTDEBUG )
		if ( s_is_capturing ) {
			s_capture_buffer += line + "\n";
		}
#endif
		util::LogHelper::Println( "    " + si.ToString() + " " + line );
		return VALUE( value::Undefined );
	} ), ep );

#if defined( DEBUG ) || defined( FASTDEBUG )

	ctx->CreateBuiltin( "global_mute", NATIVE_CALL() {
		logger::g_is_muted = true;
		return VALUE( value::Undefined );
	} ), ep );

	ctx->CreateBuiltin( "global_unmute", NATIVE_CALL() {
		logger::g_is_muted = false;
		return VALUE( value::Undefined );
	} ), ep );

#endif
}

}
}
