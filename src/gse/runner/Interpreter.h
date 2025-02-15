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

	const Value Execute( context::Context* ctx, ExecutionPointer& ep, const program::Program* program ) const override;

private:

	class Function : public type::Callable {
	public:
		Function(
			const Interpreter* runner,
			context::Context* context,
			const std::vector< std::string >& parameters,
			const program::Program* const program
		);
		~Function();
		Value Run( GSE_CALLABLE, const type::function_arguments_t& arguments ) override;
	private:
		const Interpreter* runner;
		context::Context* context;
		const std::vector< std::string > parameters;
		const program::Program* const program;
	};

	const Value EvaluateScope( context::Context* ctx, ExecutionPointer& ep, const program::Scope* scope ) const;
	const Value EvaluateStatement( context::Context* ctx, ExecutionPointer& ep, const program::Statement* statement ) const;
	const Value EvaluateConditional( context::Context* ctx, ExecutionPointer& ep, const program::Conditional* conditional, bool is_nested = false ) const;
	const Value EvaluateExpression( context::Context* ctx, ExecutionPointer& ep, const program::Expression* expression, bool* returnflag = nullptr ) const;
	const Value EvaluateOperand( context::Context* ctx, ExecutionPointer& ep, const program::Operand* operand ) const;
	const std::string EvaluateString( context::Context* ctx, ExecutionPointer& ep, const program::Operand* operand ) const;
	const Value EvaluateRange( context::Context* ctx, ExecutionPointer& ep, const program::Operand* operand, const bool only_index = false ) const;
	const bool EvaluateBool( context::Context* ctx, ExecutionPointer& ep, const program::Operand* operand ) const;
	const program::Variable* EvaluateVariable( context::Context* ctx, ExecutionPointer& ep, const program::Operand* operand ) const;
	const std::string EvaluateVarName( context::Context* ctx, ExecutionPointer& ep, const program::Operand* operand ) const;

	const Value Deref( context::Context* ctx, const si_t& si, ExecutionPointer& ep, const Value& value ) const;
	void WriteByRef( context::Context* ctx, const si_t& si, ExecutionPointer& ep, const Value& ref, const Value& value ) const;
	void ValidateRange( context::Context* ctx, const si_t& si, ExecutionPointer& ep, const type::Array* array, const std::optional< size_t > from, const std::optional< size_t > to ) const;
	void CheckBreakCondition( const gse::Value& result, bool* need_break, bool* need_clear ) const;

};

}
}
