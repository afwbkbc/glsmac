#include "Buffer.h"

#include <cstring>

namespace types {

Buffer::Buffer() {
	allocated_len = 0;
	lenw = 0;
	lenr = 0;
	data = nullptr;
	dw = nullptr;
	dr = nullptr;
}

Buffer::Buffer( const string& strval ) {
	allocated_len = strval.size();
	lenw = strval.size();
	lenr = 0;
	data = (data_t*)malloc( lenw );
	memcpy( ptr( data, 0, lenw ), strval.data(), lenw );
	dw = data + lenw;
	dr = data;
}

Buffer::~Buffer() {
	if ( data ) {
		free( data );
	}
}

Buffer::Buffer( Buffer& other) {
	allocated_len = other.allocated_len;
	lenw = other.lenw;
	lenr = other.lenr;
	if ( other.data ) {
		data = (data_t*)malloc( lenw );
		memcpy( ptr( data, 0, lenw ), ptr( other.data, 0, other.lenw ), lenw );
	}
	else {
		data = nullptr;
	}
	dw = data + lenw;
	dr = data + lenr;
}

void Buffer::Alloc( size_t size ) {
	if ( size > allocated_len ) {
		while ( size > allocated_len ) {
			allocated_len += BUFFER_ALLOC_CHUNK;
		}
		if ( data ) {
			//Log( "Reallocating " + to_string( allocated_len ) + " bytes" );
			data = (data_t*)realloc( data, allocated_len );
		}
		else { 
			//Log( "Allocating " + to_string( allocated_len ) + " bytes" );
			data = (data_t*)malloc( allocated_len );
		}
		dw = ptr( data, lenw, 0 );
		dr = ptr( data, lenr, 0 );
	}
	lenw += size;
}

// note: mostly THROWs instead of ASSERTs, because we need that validation in release mode too to prevent buffer overflows
void Buffer::WriteImpl( type_t type, const char* s, const size_t sz ) {
	//Log( "Writing " + to_string( sz ) + " bytes (type=" + to_string( type ) + ")" );
	checksum_t c = 0;
	Alloc( sizeof(type) + sizeof(sz) + sz + sizeof(c) );
	*(dw++) = type;
	memcpy( dw, &sz, sizeof( sz ) ); dw += sizeof( sz );
	
	for ( size_t i = 0 ; i < sz ; i++ ) {
		c ^= (*(dw++) = *(s++));
	}
	
	//Log( "Writing checksum (" + to_string( c ) + ")" );
	*(dw++) = c;
	
	ASSERT( dw - data == lenw, "buffer write bytes count mismatch ( " + to_string( dw - data ) + " != " + to_string( lenw ) + " )" );
	//Log( "Written successfully" );
}

char* Buffer::ReadImpl( type_t need_type, char* s, size_t* sz, const size_t need_sz ) {
	type_t type = 0;
	if ( lenw < lenr + sizeof( type ) + sizeof( *sz ) ) {
		THROW( "buffer ends prematurely (while reading header)" );
	}
	type = *(dr++);
	if ( type != need_type ) {
		THROW( "unexpected type on buffer read ( " + to_string( need_type ) + " != " + to_string( type ) + " )" );
	}
	memcpy( sz, dr, sizeof( *sz ) );
	dr += sizeof( *sz );
	if ( need_sz && ( need_sz != *sz ) ) {
		THROW( "buffer read size mismatch ( " + to_string( need_sz ) + " != " + to_string( *sz ) + " )" );
	}
	checksum_t need_c = 0;
	lenr += sizeof( type ) + sizeof( *sz ) + *sz + sizeof( need_c );
	if ( lenw < lenr ) {
		THROW( "buffer ends prematurely (while reading data)" );
	}
	//Log( "Reading " + to_string( *sz ) + " bytes (type=" + to_string( type ) + ")" );
	
	if ( s == nullptr && *sz > 0 ) {
		s = (char*)malloc( *sz );
	}
	
	char* s_ptr = s;
	
	for ( size_t i = 0 ; i < *sz ; i++ ) {
		need_c ^= (*(s_ptr++) = *(dr++));
	}
	
	//Log( "Checking checksum (" + to_string( need_c ) + ")" );
	checksum_t c = *(dr++);
	if ( need_c != c ) {
		THROW( "buffer read checksum mismatch ( " + to_string( need_c ) + " != " + to_string( c ) + " )" );
	}
	ASSERT( dr - data == lenr, "buffer read bytes count mismatch ( " + to_string( dr - data ) + " != " + to_string( lenr ) + " )" );
	//Log( "Read successfully" );
	
	return s;
}

void Buffer::WriteString( const string& strval ) {
	WriteImpl( 2, strval.data(), strval.size() );
}

const string Buffer::ReadString() {
	size_t sz = 0;
	char *res_data = ReadImpl( 2, nullptr, &sz );
	if ( sz > 0 ) {
		string result = string( res_data, sz );
		free( res_data );
		return result;
	}
	else {
		return "";
	}
}

void Buffer::WriteInt( const int32_t intval ) {
	WriteImpl( 1, (const char*)&intval, sizeof( int32_t ) );
}

const int32_t Buffer::ReadInt() {
	int32_t intval = 0;
	size_t sz = 0;
	ReadImpl( 1, (char*)&intval, &sz, sizeof( int32_t ) );
	return intval;
}

const string Buffer::ToString() const {
	return string( (const char*)data, lenw );
}

}
