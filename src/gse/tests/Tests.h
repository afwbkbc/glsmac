#pragma once

#include <string>

namespace task::gsetests {
class GSETests;
}

namespace gse {

namespace program {
class Program;
}

namespace tests {

void AddTests( task::gsetests::GSETests* task );

const std::string& GetTestFilename();
const std::string& GetTestSource();
const program::Program* GetTestProgram();
const std::string& GetExpectedResult();

}
}
