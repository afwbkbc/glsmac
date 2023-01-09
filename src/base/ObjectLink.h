#pragma once

namespace base {

class ObjectLink {
public:
	template <class T1, class T2>
	ObjectLink( const T1 *src_object, const T2 *dst_object ) : m_src_object( (const void *)src_object ), m_dst_object( (const void *)dst_object ) {
		m_removed = false;
	}
	bool Removed() const {
		return m_removed;
	}
	template <class T>
	T *GetSrcObject() const {
		return (T *)m_src_object;
	}
	template <class T>
	T *GetDstObject() const {
		return (T *)m_dst_object;
	}
	void Remove() {
		m_removed = true;
	}
protected:
	const void *m_src_object;
	const void *m_dst_object;
	bool m_removed;
};

} /* namespace base */
