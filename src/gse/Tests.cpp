#include "Tests.h"

#include "tests/GSE.h"
#include "tests/Parser.h"

namespace gse {

void AddTests( task::gsetests::GSETests* task ) {

	task->AddTest(
		"test if tests work",
		GT() {
			GT_OK();
		}
	);

	tests::AddGSETests( task );
	tests::AddParserTests( task );

}

}
