#pragma once

#include "Mocks.h"

namespace gse {
namespace tests {
namespace mocks {

class Console {
public:

	static void AddMocks( gse::Context* ctx );

	// make sure that passed callable is actually Console::Log object!
	static const std::string& GetLogsOf( const type::Callable* const console_log_object );

private:

	class Loggable : public type::Callable {
	protected:
		const std::string ArgumentsToString( const Callable::function_arguments_t& arguments, bool use_dump ) const;
	};

	class Log : public Loggable {
	public:
		Value Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) override;
		const std::string& GetOutput() const;
	private:
		std::string m_output = "";
	};

	class Dump : public Loggable {
	public:
		Value Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) override;
	};

	class GlobalMute : public Loggable {
	public:
		Value Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) override;
	};

	class GlobalUnmute : public Loggable {
	public:
		Value Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) override;
	};

};

}
}
}
