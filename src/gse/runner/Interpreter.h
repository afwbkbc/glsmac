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

	void Execute( Context* ctx, const program::Program* program ) const override;

private:

	class Function : public type::Callable {
	public:
		Function( const program::Program* program );
		Value Run( GSE* gse, const Callable::function_arguments_t arguments ) override;
	private:
		const program::Program* program;
	};

	void ExecuteScope( Context* ctx, const program::Scope* scope ) const;
	void ExecuteStatement( Context* ctx, const program::Statement* statement ) const;
	const Value EvaluateExpression( Context* ctx, const program::Expression* expression ) const;
	const Value EvaluateOperand( Context* ctx, const program::Operand* operand ) const;
	const std::string& EvaluateString( Context* ctx, const program::Operand* operand ) const;
	const program::Variable* EvaluateVariable( Context* ctx, const program::Operand* operand ) const;
	const std::string EvaluateVarName( Context* ctx, const program::Operand* operand ) const;

	const Value& Deref( const Value& value ) const;
	void WriteByRef( const gse::Value& ref, const gse::Value& value ) const;

};

}
}
