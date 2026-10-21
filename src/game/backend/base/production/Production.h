#pragma once

#include "types/Buffer.h"

namespace gse::value {
class Object;
}

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

	Production( const production_type_t type, const std::string& hint );
	virtual ~Production() = default;

	void Serialize( types::Buffer& buf ) const;
	static const Production* const Deserialize( types::Buffer& buf );

	virtual const uint8_t GetGroupOrder() const = 0;
	virtual gse::value::Object* const ToValue( GSE_CALLABLE, const std::string& id ) const = 0;

protected:
	const std::string m_hint;

	virtual void SerializeImpl( types::Buffer& buf ) const = 0;

private:
	const production_type_t m_type;
};

}
}
}
}
