#pragma once

#include "Base.h"

namespace base {

CLASS( ObjectLink, base::Base )
	template< class T1, class T2 >
	ObjectLink( const T1* src_object, const T2* dst_object )
		: base::Base()
		, m_src_object( (const void*)src_object )
		, m_dst_object( (const void*)dst_object ) {
		//
	}
	bool Removed() const {
		return m_removed;
	}
	template< class T >
	T* GetSrcObject() const {
		return (T*)m_src_object;
	}
	template< class T >
	T* GetDstObject() const {
		return (T*)m_dst_object;
	}
	void Remove() {
		m_removed = true;
	}
protected:
	const void* m_src_object = nullptr;
	const void* m_dst_object = nullptr;
	bool m_removed = false;
};

} /* namespace base */
