#include "GSE.h"

#include "gse/Tests.h"
#include "gse/Context.h"

#include "gse/type/Undefined.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/String.h"
#include "gse/type/Object.h"

#include "gse/runner/Interpreter.h"

namespace gse {
namespace tests {

using namespace program;

void AddRunnerTests( task::gsetests::GSETests* task ) {

	const Program* test_program = GetTestProgram();

	// to test execution output
	class ConsoleLogMock : public type::Callable {
	public:
		Value Run( GSE* gse, const Callable::function_arguments_t& arguments ) override {
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

	const std::string expected_output = "null{}\n"
										"bool{true}\n"
										"bool{true} bool{false}\n"
										"bool{true} bool{true} bool{true} bool{false}\n"
										"bool{true} bool{false}\n"
										"bool{false} bool{true} bool{true} bool{true}\n"
										"bool{false} bool{true} bool{false} bool{true}\n"
										"bool{true} bool{false} bool{true} bool{true}\n"
										"bool{true}\n"
										"45 -61\n"
										"52 10\n"
										"CHILD VALUE\n"
										"bool{true}\n"
										"object{propertyInt=int{372}} object{propertyInt1=int{150},propertyInt2=int{222},propertyString=string{STRING}}\n"
										"bye!\n";

#define VALIDATE() { \
    const std::string actual_output = ( (ConsoleLogMock*)console_log_mock.Get() )->output; \
    GT_ASSERT( actual_output == expected_output ); \
}
	task->AddTest(
		"test if interpreter executes programs correctly",
		GT( test_program, expected_output ) {

			runner::Interpreter interpreter;

			Context context;

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
