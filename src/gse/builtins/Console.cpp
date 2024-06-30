#include "Console.h"

#include <iostream>

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/type/Undefined.h"

#include "logger/Stdout.h"

namespace gse {
namespace builtins {

#ifdef DEBUG
static bool s_is_capturing = false;
static std::string s_capture_buffer = "";
void Console::CaptureStart() const {
	ASSERT_NOLOG( !s_is_capturing, "already capturing" );
	s_capture_buffer.clear();
	s_is_capturing = true;
}
const std::string& Console::CaptureStopGet() const {
	ASSERT_NOLOG( s_is_capturing, "not capturing" );
	s_is_capturing = false;
	return s_capture_buffer;
}
#endif

void Console::AddToContext( context::Context* ctx ) {

	ctx->CreateBuiltin( "print", NATIVE_CALL( this ) {
		std::string line = "";
		for ( const auto& it : arguments ) {
			if ( !line.empty() ) {
				line += " ";
			}
			line += it.Get()->ToString();
		}
#ifdef DEBUG
		if ( s_is_capturing ) {
			s_capture_buffer += line + "\n";
		}
#endif
		std::cout << "    " << call_si.ToString() << " " << line << std::endl;
		return VALUE( type::Undefined );
	} ) );

#ifdef DEBUG

	ctx->CreateBuiltin( "global_mute", NATIVE_CALL() {
		logger::g_is_muted = true;
		return VALUE( type::Undefined );
	} ) );

	ctx->CreateBuiltin( "global_unmute", NATIVE_CALL() {
		logger::g_is_muted = false;
		return VALUE( type::Undefined );
	} ) );

#endif
}

}
}
