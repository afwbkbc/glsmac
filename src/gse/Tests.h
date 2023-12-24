#pragma once

#include "task/gsetests/GSETests.h"

#include "gse/program/Program.h"

namespace gse {

void AddTests( task::gsetests::GSETests* task );
const program::Program* GetTestProgram();

}
