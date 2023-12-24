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
	class ConsoleLog : public type::Callable {
	public:
		Value Run( GSE* gse, const Callable::function_arguments_t arguments ) override {
			for ( const auto& it : arguments ) {
				const auto* arg = it.Get();
				switch ( arg->type ) {
					case Type::T_INT: {
						output += std::to_string( ( (type::Int*)arg )->value );
						break;
					}
					case Type::T_FLOAT: {
						output += std::to_string( ( (type::Float*)arg )->value );
						break;
					}
					case Type::T_STRING: {
						output += ( (type::String*)arg )->value;
						break;
					}
					default: {
						output += arg->ToString();
					}
				}
				output += "\n";
			}
			return VALUE( type::Undefined );
		}
		std::string output = "";
	};

	const std::string expected_output = "CHILD VALUE\n"
										"bool{0}\n"
										"object{propertyInt=int{372}}\n"
										"object{propertyInt1=int{150},propertyInt2=int{222},propertyString=string{STRING}}\n"
										"bye!\n";

#define VALIDATE() { \
    const std::string actual_output = ( (ConsoleLog*)console_log.Get() )->output; \
    GT_ASSERT( actual_output == expected_output ); \
}
	task->AddTest(
		"test if interpreter executes programs correctly",
		GT( test_program, expected_output ) {

			runner::Interpreter interpreter;

			Context context;

			// add some mocks
			const auto console_log = VALUE( ConsoleLog );
			type::Object::properties_t console_properties = {
				{
					"log",
					console_log
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
