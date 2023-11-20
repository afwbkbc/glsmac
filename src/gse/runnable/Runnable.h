#pragma once

namespace gse {
namespace runnable {

class Runnable {
public:

	virtual ~Runnable() {}

	virtual void Run() = 0;

};

}
}
