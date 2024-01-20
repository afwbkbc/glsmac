#pragma once

#include <unordered_map>
#include <string>
#include <optional>

#include "Runner.h"

#include "gse/Value.h"

#include "gse/program/Variable.h"
#include "gse/program/Conditional.h"
#include "gse/program/Statement.h"

#include "gse/type/Callable.h"
#include "gse/type/Array.h"

namespace gse {
namespace runner {

CLASS( Interpreter, Runner )

	const Value Execute( Context* ctx, const program::Program* program ) const override;

private:

	class Function : public type::Callable {
	public:
		Function(
			const Interpreter* runner,
			Context const* context,
			const std::vector< std::string >& parameters,
			const program::Program* const program
		);
		Value Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) override;
	private:
		const Interpreter* runner;
		Context const* context;
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
	void WriteByRef( const Context* ctx, const si_t& si, const gse::Value& ref, const gse::Value& value ) const;
	void ValidateRange( const Context* ctx, const si_t& si, const type::Array* array, const std::optional< size_t > from, const std::optional< size_t > to ) const;

};

}
}
