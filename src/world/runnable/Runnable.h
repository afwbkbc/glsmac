#pragma once

namespace world {
namespace runnable {

class Runnable {
public:

	virtual ~Runnable() {}

	virtual void Run() = 0;

};

}
}
