#pragma once

#include "gse/Bindings.h"

#include "gse/ChildContext.h"

#include "Console.h"
#include "Include.h"

namespace gse {
namespace builtins {

class Builtins : public Bindings {
public:
	void AddToContext( gse::Context* ctx ) const override;

#ifdef DEBUG
	void LogCaptureStart() const { m_console.CaptureStart(); }
	const std::string& LogCaptureStopGet() const { return m_console.CaptureStopGet(); }
#endif

private:
	const Include m_include = {};
	const Console m_console = {};

};

}
}
