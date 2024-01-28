#pragma once

#include <string>

#include "gse/ChildContext.h"

#include "gse/Bindings.h"

namespace gse {
namespace builtins {

class Console : public Bindings {
public:
	void AddToContext( gse::Context* ctx ) override;

#ifdef DEBUG
	void CaptureStart() const;
	const std::string& CaptureStopGet() const;
#endif

private:
	const std::string ArgumentsToString( const std::vector< Value >& arguments, bool use_dump ) const;

};

}
}
