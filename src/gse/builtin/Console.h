#pragma once

#include <string>

#include "gse/ChildContext.h"

#include "Builtin.h"

namespace gse {
namespace builtin {

class Console : public Builtin {
public:
	void AddToContext( gse::Context* ctx ) const override;

#ifdef DEBUG
	void CaptureStart() const;
	const std::string& CaptureStopGet() const;
#endif

private:
	const std::string ArgumentsToString( const std::vector< Value >& arguments, bool use_dump ) const;

};

}
}
