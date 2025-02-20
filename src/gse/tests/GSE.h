#pragma once

namespace gc {
class Space;
}

namespace task::gsetests {
class GSETests;
}

namespace gse {
namespace tests {

void AddGSETests( gc::Space* const gc_space, task::gsetests::GSETests* task );

}
}
