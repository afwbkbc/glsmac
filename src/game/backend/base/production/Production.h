#pragma once

#include "types/Buffer.h"

namespace game {
namespace backend {
namespace base {
namespace production {

class Production {
public:
	enum production_type_t {
		PT_FACILITY,
		PT_CUSTOM,
	};

	Production( const production_type_t type );
	virtual ~Production() = default;

	void Serialize( types::Buffer& buf ) const;
	static const Production* const Deserialize( types::Buffer& buf );

protected:
	virtual void SerializeImpl( types::Buffer& buf ) const = 0;

private:
	const production_type_t m_type;

};

}
}
}
}
