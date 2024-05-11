#pragma once

#include "base/Base.h"

namespace rr {

enum type_t {
	RR_NONE,
	RR_GETDATA,
	RR_CAPTURE,
};

// Request-Response
CLASS( RR, base::Base )

	const type_t GetType() const;
	const bool IsProcessed() const;
	void SetProcessed();

	RR() = delete;
	virtual ~RR();

protected:
	RR( const type_t type );

private:
	const type_t m_type = RR_NONE;
	bool m_is_processed = false;

};

}
