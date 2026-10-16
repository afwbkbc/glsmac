#pragma once

#include "Production.h"

namespace game {
namespace backend {
namespace base {

class FacilityDef;

namespace production {

class Facility : public Production {
public:
	Facility( const FacilityDef* const def );
	virtual ~Facility() = default;

private:
	friend class Production;
	static const Production* const DeserializeImpl( types::Buffer& buf );

protected:
	void SerializeImpl( types::Buffer& buf ) const override;

private:
	const FacilityDef* const m_def;

};

}
}
}
}
