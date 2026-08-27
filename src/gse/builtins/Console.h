#pragma once

#include <string>

#include "gse/Bindings.h"

namespace gse {
namespace builtins {

class Console : public Bindings {
public:
	void AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) override;

#if defined( DEBUG ) || defined( FASTDEBUG ) || defined( GLSMAC_TESTING )
	void CaptureStart() const;
	const std::string& CaptureStopGet() const;
#endif

};

}
}
