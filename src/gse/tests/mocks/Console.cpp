#include "Console.h"

#include <iostream>

#include "gse/type/Object.h"
#include "gse/type/Undefined.h"

namespace gse {
namespace tests {
namespace mocks {

void Console::AddMocks( gse::Context* ctx ) {
	type::Object::properties_t mocks = {
		{
			"log",
			VALUE( Log )
		}
	};
	ctx->CreateVariable( "console", VALUE( type::Object, mocks ) );
}

const std::string& Console::GetLogsOf( const type::Callable* const console_log_object ) {
	return ( (mocks::Console::Log*)console_log_object )->GetOutput();
}

const std::string Console::Loggable::ArgumentsToString( const Callable::function_arguments_t& arguments, bool use_dump ) const {
	std::string line = "";
	for ( const auto& it : arguments ) {
		if ( !line.empty() ) {
			line += " ";
		}
		line += use_dump
			? it.Get()->Dump()
			: it.Get()->ToString();
	}
	return line;
}

Value Console::Log::Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) {
	const auto line = ArgumentsToString( arguments, false );
	m_output += line + "\n";
	std::cout << "    " << call_si.ToString() << " " << line << std::endl;
	return VALUE( type::Undefined );
}

const std::string& Console::Log::GetOutput() const {
	return m_output;
}

Value Console::Dump::Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) {
	std::cout << "    " << call_si.ToString() << " " << ArgumentsToString( arguments, true ) << std::endl;
	return VALUE( type::Undefined );
}

}
}
}
