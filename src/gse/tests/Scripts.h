#pragma once

namespace task::gsetests {
class GSETests;
}

namespace gse {
namespace tests {

void AddScriptsTests( gc::Space* const gc_space, task::gsetests::GSETests* task );

}
}
