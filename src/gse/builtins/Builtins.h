#pragma once

#include "gse/Bindings.h"

// TODO: refactor
#include "Common.h"
#include "Console.h"
#include "Include.h"
#include "Conversions.h"
#include "Math.h"
#include "String.h"
#include "Async.h"

namespace gse {
namespace builtins {

class Builtins : public Bindings {
public:
	void AddToContext( context::Context* ctx ) override;

#ifdef DEBUG
	void LogCaptureStart() const { m_console.CaptureStart(); }
	const std::string& LogCaptureStopGet() const { return m_console.CaptureStopGet(); }
#endif

private:
	Common m_common = {};
	Include m_include = {};
	Console m_console = {};
	Conversions m_conversions = {};
	Math m_math = {};
	String m_string = {};
	Async m_async = {};

};

}
}
