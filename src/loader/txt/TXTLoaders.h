#pragma once

namespace loader {
namespace txt {

class FactionTXTLoader;

class TXTLoaders {
public:
	TXTLoaders();
	~TXTLoaders();

	FactionTXTLoader* const factions;
};

}
}
