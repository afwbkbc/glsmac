#pragma once

#include "gse/Bindings.h"

#include "gse/ChildContext.h"

#include "Common.h"
#include "Console.h"
#include "Include.h"
#include "Conversions.h"

namespace gse {
namespace builtins {

class Builtins : public Bindings {
public:
	void AddToContext( gse::Context* ctx ) override;

#ifdef DEBUG
	void LogCaptureStart() const { m_console.CaptureStart(); }
	const std::string& LogCaptureStopGet() const { return m_console.CaptureStopGet(); }
#endif

private:
	Common m_common = {};
	Include m_include = {};
	Console m_console = {};
	Conversions m_conversions = {};

};

}
}
