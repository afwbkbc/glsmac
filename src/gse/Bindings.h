#pragma once

namespace gse {

class Context;

class Bindings {
public:
	virtual void AddToContext( gse::Context* ctx ) = 0;
};

}
