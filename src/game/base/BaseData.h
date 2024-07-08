#pragma once

#include "gse/Wrappable.h"
#include "gse/Value.h"

#include "types/Buffer.h"

namespace game {
namespace base {

class BaseData : public gse::Wrappable {
public:

	BaseData( const std::string& name, const size_t population );
	BaseData( types::Buffer& buf );

	std::string name;
	size_t population;

	void Serialize( types::Buffer& buf ) const;
	void Unserialize( types::Buffer& buf );

	const std::string ToString( const std::string& prefix = "" ) const;

	WRAPDEFS_DYNAMIC( BaseData );

};

}
}
