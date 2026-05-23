#pragma once

#include "gse/Value.h"

namespace gse {
class GCWrappable;
namespace value {
class Object;
}
}

namespace game {
namespace backend {

namespace resource {
class ResourceManager;
}

class ResourceRelated {
protected:
	gse::value::Object* const GetResourcesFromCallback( GSE_CALLABLE, gse::GCWrappable* const object, resource::ResourceManager* const rm, const std::string& event, const gse::f_args_t& f_args );

};

}
}
