#include "GSE.h"

#include "Tests.h"

#include "gse/Context.h"

#include "gse/type/Undefined.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/String.h"
#include "gse/type/Object.h"

#include "gse/runner/Interpreter.h"

#include "util/String.h"

namespace gse {
namespace tests {

using namespace program;

void AddRunnerTests( task::gsetests::GSETests* task ) {

	const auto& test_program = GetTestProgram();

	// to test execution output
	class ConsoleLogMock : public type::Callable {
	public:
		Value Run( const Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) override {
			std::string line = "";
			for ( const auto& it : arguments ) {
				if ( !line.empty() ) {
					line += " ";
				}
				const auto* arg = it.Get();
				switch ( arg->type ) {
					case Type::T_INT: {
						line += std::to_string( ( (type::Int*)arg )->value );
						break;
					}
					case Type::T_FLOAT: {
						line += std::to_string( ( (type::Float*)arg )->value );
						break;
					}
					case Type::T_STRING: {
						line += ( (type::String*)arg )->value;
						break;
					}
					default: {
						line += arg->ToString();
					}
				}
			}
			output += line + "\n";
			return VALUE( type::Undefined );
		}
		std::string output = "";
	};

	const std::string expected_output = GetExpectedResult();

#define VALIDATE() { \
    const std::string actual_output = ( (ConsoleLogMock*)console_log_mock.Get() )->output; \
    GT_ASSERT( actual_output == expected_output, ", actual output:\n" + actual_output ); \
}
	task->AddTest(
		"test if interpreter executes programs correctly",
		GT( test_program, expected_output ) {

			runner::Interpreter interpreter;

			Context context( nullptr, util::String::SplitToLines( GetTestSource() ), {} );

			// add some mocks
			const auto console_log_mock = VALUE( ConsoleLogMock );
			type::Object::properties_t console_properties = {
				{
					"log",
					console_log_mock
				}
			};
			const auto console = VALUE( type::Object, console_properties );
			context.CreateVariable( "console", console );

			interpreter.Execute( &context, test_program );

			VALIDATE();

			GT_OK();
		}
	);

}

}
}
