#pragma once

#include "task/gsetests/GSETests.h"

#include "gse/program/Program.h"

namespace gse {
namespace tests {

void AddTests( task::gsetests::GSETests* task );

const std::string& GetTestFilename();
const std::string& GetTestSource();
const program::Program* GetTestProgram();
const std::string& GetExpectedResult();

}
}
