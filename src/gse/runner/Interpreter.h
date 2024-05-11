#pragma once

#include <string>
#include <optional>

#include "Runner.h"

#include "gse/type/Types.h"

#include "gse/Value.h"
#include "gse/type/Callable.h"

namespace gse {

namespace program {
class Scope;
class Statement;
class Control;
class Conditional;
class Expression;
class Operand;
class Variable;
}

namespace type {
class Array;
}

namespace runner {

CLASS( Interpreter, Runner )

	const Value Execute( Context* ctx, const program::Program* program ) const override;

private:

	class Function : public type::Callable {
	public:
		Function(
			const Interpreter* runner,
			Context* context,
			const std::vector< std::string >& parameters,
			const program::Program* const program
		);
		~Function();
		Value Run( Context* ctx, const si_t& call_si, const type::function_arguments_t& arguments ) override;
	private:
		const Interpreter* runner;
		Context* context;
		const std::vector< std::string > parameters;
		const program::Program* const program;
	};

	const Value EvaluateScope( Context* ctx, const program::Scope* scope ) const;
	const Value EvaluateStatement( Context* ctx, const program::Statement* statement ) const;
	const gse::Value EvaluateConditional( Context* ctx, const program::Conditional* conditional, bool is_nested = false ) const;
	const Value EvaluateExpression( Context* ctx, const program::Expression* expression, bool* returnflag = nullptr ) const;
	const Value EvaluateOperand( Context* ctx, const program::Operand* operand ) const;
	const std::string EvaluateString( Context* ctx, const program::Operand* operand ) const;
	const Value EvaluateRange( Context* ctx, const program::Operand* operand, const bool only_index = false ) const;
	const bool EvaluateBool( Context* ctx, const program::Operand* operand ) const;
	const program::Variable* EvaluateVariable( Context* ctx, const program::Operand* operand ) const;
	const std::string EvaluateVarName( Context* ctx, const program::Operand* operand ) const;

	const Value Deref( Context* ctx, const si_t& si, const Value& value ) const;
	void WriteByRef( Context* ctx, const si_t& si, const gse::Value& ref, const gse::Value& value ) const;
	void ValidateRange( Context* ctx, const si_t& si, const type::Array* array, const std::optional< size_t > from, const std::optional< size_t > to ) const;

};

}
}
