#include "ResourceRelated.h"

#include "game/backend/resource/ResourceManager.h"

namespace game {
namespace backend {

gse::value::Object* const ResourceRelated::GetResourcesFromCallback( GSE_CALLABLE, gse::GCWrappable* const object, resource::ResourceManager* const rm, const std::string& event, const gse::f_args_t& f_args ) {
	auto* const result = object->Trigger( GSE_CALL, event, f_args );
	ASSERT( result, "GetResources result is null" );
	if ( result->type != gse::VT_OBJECT ) {
		GSE_ERROR( gse::EC.GAME_ERROR, event + " must return object, got: " + result->ToString() );
	}
	const auto& defined_resources = rm->GetDefinedResources();
	auto* obj = ( (gse::value::Object*)result );
	auto& v = obj->value;
	for ( const auto& it : v ) {
		if ( defined_resources.find( it.first ) == defined_resources.end() ) {
			GSE_ERROR( gse::EC.GAME_ERROR, event + " returned unknown resource: " + it.first );
		}
		if ( it.second->type != gse::VT_INT ) {
			GSE_ERROR( gse::EC.GAME_ERROR, event + " returned invalid value for resource " + it.first + ", expected: Int, got " + it.second->GetTypeString() + ": " + it.second->ToString() );
		}
		const auto& value = ( (gse::value::Int*)it.second )->value;
		if ( value < 0 || value > 255 ) {
			GSE_ERROR( gse::EC.GAME_ERROR, event + " returned invalid value for resource " + it.first + ", must be between 0 and 255, got: " + std::to_string( value ) );
		}
	}
	for ( const auto& it : defined_resources ) {
		if ( v.find( it.first ) == v.end() ) {
			obj->Assign( it.first, VALUE( gse::value::Int, , 0 ) );
		}
	}
	return obj;
}

}
}
