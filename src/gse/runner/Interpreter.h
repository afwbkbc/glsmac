#pragma once

#include <unordered_map>
#include <string>

#include "Runner.h"

#include "gse/Value.h"

#include "gse/program/Variable.h"

#include "gse/type/Callable.h"

namespace gse {
namespace runner {

CLASS( Interpreter, Runner )

	const Value Execute( Context* ctx, const program::Program* program ) const override;

private:

	class Function : public type::Callable {
	public:
		Function(
			const Interpreter* runner,
			Context const* parent_context,
			const std::vector< std::string >& parameters,
			const program::Program* const program
		);
		Value Run( GSE* gse, const Callable::function_arguments_t& arguments ) override;
	private:
		const Interpreter* runner;
		Context const* parent_context;
		const std::vector< std::string > parameters;
		const program::Program* const program;
	};

	const Value ExecuteScope( Context* ctx, const program::Scope* scope ) const;
	const Value ExecuteStatement( Context* ctx, const program::Statement* statement ) const;
	const Value EvaluateExpression( Context* ctx, const program::Expression* expression, bool* returnflag = nullptr ) const;
	const Value EvaluateOperand( Context* ctx, const program::Operand* operand ) const;
	const std::string& EvaluateString( Context* ctx, const program::Operand* operand ) const;
	const bool EvaluateBool( Context* ctx, const program::Operand* operand ) const;
	const program::Variable* EvaluateVariable( Context* ctx, const program::Operand* operand ) const;
	const std::string EvaluateVarName( Context* ctx, const program::Operand* operand ) const;

	const Value& Deref( const Value& value ) const;
	void WriteByRef( const gse::Value& ref, const gse::Value& value ) const;

};

}
}
