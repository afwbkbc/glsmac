#pragma once

#include "base/Base.h"

using namespace std;

namespace types {
	
CLASS( Buffer, base::Base )
	
	static const size_t BUFFER_ALLOC_CHUNK = 1024;
	
	typedef uint8_t data_t;
	typedef char checksum_t;
	typedef char type_t;
	
	Buffer();
	Buffer( const string& strval );
	~Buffer();
	
	Buffer( Buffer& other);
	
	data_t* data;
	data_t* dw;
	data_t* dr;
	size_t allocated_len;
	size_t lenw;
	size_t lenr;
	checksum_t checksum;
	
	void WriteString( const string& strval );
	const string ReadString();
	void WriteInt( const int32_t intval ); // todo: size and sign parameters?
	const int32_t ReadInt();
	
	const string ToString() const;
	
private:
	void WriteImpl( const type_t type, const char* s, const size_t sz );
	char* ReadImpl( const type_t need_type, char* s, size_t* sz, const size_t need_sz = 0 );
	void Alloc( size_t size );
	
};

}
