#include "Main.h"

#include "GLSMAC.h"

namespace task {
namespace main {

void Main::Start() {

	NEW( m_glsmac, GLSMAC );

	m_glsmac->RunMain();

}

void Main::Stop() {

	DELETE( m_glsmac );

}

void Main::Iterate() {

}

}
}
