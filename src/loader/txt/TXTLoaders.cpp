#include "TXTLoaders.h"

#include "FactionTXTLoader.h"

namespace loader {
namespace txt {

TXTLoaders::TXTLoaders()
	: factions( new FactionTXTLoader() ) {
	//
}
TXTLoaders::~TXTLoaders() {
	delete factions;
}

}
}