#pragma once

#include <string>
#include <vector>

#include "Morale.h"
#include "types/Buffer.h"

namespace game {
namespace backend {
namespace unit {

class MoraleSet {
public:
	typedef std::vector< Morale > morale_values_t;
	MoraleSet( const std::string& id, const morale_values_t& morale_values );

	const std::string m_id;
	const morale_values_t m_morale_values;

	const std::string ToString( const std::string& prefix ) const;

	static const types::Buffer Serialize( const MoraleSet* moraleset );
	static MoraleSet* Unserialize( types::Buffer& buf );

};

}
}
}
