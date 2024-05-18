#pragma once

namespace gse {

namespace context {
class Context;
}

class Bindings {
public:
	virtual void AddToContext( context::Context* ctx ) = 0;
};

}
