#include "Object.h"

#include "Space.h"

namespace gc {

Object::Object( gc::Space* const gc_space ) {
	gc_space->Add( this );
}

void Object::GetReachableObjects( std::unordered_set< Object* >& active_objects ) {
	active_objects.insert( this );
}

const std::string Object::GCString() const {
	return std::to_string( (unsigned long)this );
}

}
