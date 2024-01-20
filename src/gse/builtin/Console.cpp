#include "Console.h"

#include "gse/callable/Native.h"

#include "gse/type/Object.h"
#include "gse/type/Undefined.h"

#include "logger/Stdout.h"

namespace gse {
namespace builtin {

void Console::AddToContext( gse::Context* ctx ) const {

	type::Object::properties_t properties = {
		{
			"log",
			NATIVE_CALL( this ) {
				std::cout << "    " << call_si.ToString() << " " << ArgumentsToString( arguments, false ) << std::endl;
				return VALUE( type::Undefined );
			} )
		},
		{
			"dump",
			NATIVE_CALL( this ) {
				std::cout << "    " << call_si.ToString() << " " << ArgumentsToString( arguments, true ) << std::endl;
				return VALUE( type::Undefined );
			} )
		},
#ifdef DEBUG
		{
			"global_mute",
			NATIVE_CALL() {
				logger::g_is_muted = true;
				return VALUE( type::Undefined );
			} )
		},
		{
			"global_unmute",
			NATIVE_CALL() {
				logger::g_is_muted = false;
				return VALUE( type::Undefined );
			} )
		},
#endif
	};
	ctx->CreateVariable( "console", VALUE( type::Object, properties ), nullptr );
}

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

const std::string Console::ArgumentsToString( const std::vector< Value >& arguments, bool use_dump ) const {
	std::string line = "";
	for ( const auto& it : arguments ) {
		if ( !line.empty() ) {
			line += " ";
		}
		line += use_dump
			? it.Get()->Dump()
			: it.Get()->ToString();
	}
#ifdef DEBUG
	if ( s_is_capturing ) {
		s_capture_buffer += line + "\n";
	}
#endif
	return line;
}

}
}
