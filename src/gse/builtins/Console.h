#pragma once

#include <string>

#include "gse/Bindings.h"

namespace gse {
namespace builtins {

class Console : public Bindings {
public:
	void AddToContext( context::Context* ctx, ExecutionPointer& ep ) override;

#ifdef DEBUG
	void CaptureStart() const;
	const std::string& CaptureStopGet() const;
#endif

};

}
}
