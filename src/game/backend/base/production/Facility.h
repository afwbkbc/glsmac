#pragma once

#include "Production.h"

namespace game {
namespace backend {
namespace base {

class FacilityDef;

namespace production {

class Facility : public Production {
public:
	Facility( const std::string& id, const FacilityDef* const def, const std::string& hint, const std::string& intent );
	virtual ~Facility() = default;

	const uint8_t GetGroupOrder() const override;
	gse::value::Object* const ToValue( GSE_CALLABLE ) const override;

private:
	friend class Production;
	static const Production* const DeserializeImpl( types::Buffer& buf, const std::string& id, const std::string& hint, const std::string& intent );

protected:
	void SerializeImpl( types::Buffer& buf ) const override;

private:
	const FacilityDef* const m_def;

};

}
}
}
}
