#pragma once

#include "Object.h"

namespace scene::actor {
class Text;
}

namespace ui {
namespace dom {

class Text : public Object {
public:
	Text( DOM_ARGS );
	~Text();

private:
	scene::actor::Text* m_actor = nullptr;

};

}
};
