#include <cstring>

#include "Buffer.h"

namespace types {

Buffer::Buffer() {
	allocated_len = 0;
	lenw = 0;
	lenr = 0;
	data = nullptr;
	dw = nullptr;
	dr = nullptr;
}

Buffer::Buffer( const std::string& val ) {
	allocated_len = val.size();
	lenw = val.size();
	lenr = 0;
	data = (data_t*)malloc( lenw );
	memcpy( ptr( data, 0, lenw ), val.data(), lenw );
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
	const size_t new_size = lenw + size;
	if ( new_size > allocated_len ) {
		while ( new_size > allocated_len ) {
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
	lenw = new_size;
}

// note: mostly THROWs instead of ASSERTs, because we need that validation in release mode too to prevent buffer overflows
void Buffer::WriteImpl( type_t type, const char* s, const size_t sz ) {
	ASSERT( type > T_NONE && type < T_MAX, "invalid buffer write type " + std::to_string( type ) );
	//Log( "Writing " + to_string( sz ) + " bytes (type=" + to_string( type ) + ")" );
	checksum_t c = 0;
	Alloc( sizeof(type) + sizeof(sz) + sz + sizeof(c) );
	memcpy( dw, &type, sizeof( type ) ); dw += sizeof( type );
	memcpy( dw, &sz, sizeof( sz ) ); dw += sizeof( sz );
	
	for ( size_t i = 0 ; i < sz ; i++ ) {
		c ^= (*(dw++) = *(s++));
	}
	
	//Log( "Writing checksum (" + to_string( c ) + ")" );
	*(dw++) = c;
	
	ASSERT( dw - data == lenw, "buffer write bytes count mismatch ( " + std::to_string( dw - data ) + " != " + std::to_string( lenw ) + " )" );
	//Log( "Written successfully" );
}

char* Buffer::ReadImpl( type_t need_type, char* s, size_t* sz, const size_t need_sz ) {
	ASSERT( need_type > T_NONE && need_type < T_MAX, "invalid buffer read type " + std::to_string( need_type ) );
	type_t type = T_NONE;
	if ( lenw < lenr + sizeof( type ) + sizeof( *sz ) ) {
		THROW( "buffer ends prematurely (while reading header)" );
	}
	memcpy( &type, dr, sizeof( type ) ); dr += sizeof( type );
	if ( type != need_type ) {
		THROW( "unexpected type on buffer read ( " + std::to_string( need_type ) + " != " + std::to_string( type ) + " )" );
	}
	memcpy( sz, dr, sizeof( *sz ) );
	dr += sizeof( *sz );
	if ( need_sz && ( need_sz != *sz ) ) {
		THROW( "buffer read size mismatch ( " + std::to_string( need_sz ) + " != " + std::to_string( *sz ) + " )" );
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
		THROW( "buffer read checksum mismatch ( " + std::to_string( need_c ) + " != " + std::to_string( c ) + " )" );
	}
	ASSERT( dr - data == lenr, "buffer read bytes count mismatch ( " + std::to_string( dr - data ) + " != " + std::to_string( lenr ) + " )" );
	//Log( "Read successfully" );
	
	return s;
}

void Buffer::WriteBool( const bool val ) {
	const uint8_t bval = val ? 1 : 0;
	WriteImpl( T_BOOL, (const char*)&bval, sizeof( bval ) );
}

const bool Buffer::ReadBool() {
	bool boolval = false;
	size_t sz = 0;
	ReadImpl( T_BOOL, (char*)&boolval, &sz, sizeof( boolval ) );
	return boolval;
}

void Buffer::WriteInt( const long long int val ) {
	WriteImpl( T_INT, (const char*)&val, sizeof( val ) );
}

const long long int Buffer::ReadInt() {
	long long int val = 0;
	size_t sz = 0;
	ReadImpl( T_INT, (char*)&val, &sz, sizeof( val ) );
	return val;
}

void Buffer::WriteFloat( const float val ) {
	WriteImpl( T_FLOAT, (const char*)&val, sizeof( val ) );
}

const float Buffer::ReadFloat() {
	float val = 0;
	size_t sz = 0;
	ReadImpl( T_FLOAT, (char*)&val, &sz, sizeof( val ) );
	return val;
}

void Buffer::WriteString( const std::string& val ) {
	WriteImpl( T_STRING, val.data(), val.size() );
}

const std::string Buffer::ReadString() {
	size_t sz = 0;
	char *res_data = ReadImpl( T_STRING, nullptr, &sz );
	if ( sz > 0 ) {
		std::string result = std::string( res_data, sz );
		free( res_data );
		return result;
	}
	else {
		return "";
	}
}

void Buffer::WriteVec2f( const Vec2< float > val ) {
	WriteImpl( T_VEC2F, (const char*)&val, sizeof( val ) );
}

const Vec2< float > Buffer::ReadVec2f() {
	Vec2< float > val = { 0, 0 };
	size_t sz = 0;
	ReadImpl( T_VEC2F, (char*)&val, &sz, sizeof( val ) );
	return val;
}

void Buffer::WriteVec3( const Vec3 val ) {
	WriteImpl( T_VEC3, (const char*)&val, sizeof( val ) );
}

const Vec3 Buffer::ReadVec3() {
	Vec3 val;
	size_t sz = 0;
	ReadImpl( T_VEC3, (char*)&val, &sz, sizeof( val ) );
	return val;
}

void Buffer::WriteColor( const Color val ) {
	WriteImpl( T_COLOR, (const char*)&val, sizeof( val ) );
}

const Color Buffer::ReadColor() {
	Color val;
	size_t sz = 0;
	ReadImpl( T_COLOR, (char*)&val, &sz, sizeof( val ) );
	return val;
}

void Buffer::WriteData( const void* data, const size_t len ) {
	WriteImpl( T_DATA, (const char*)data, len );
}

const void* Buffer::ReadData( const size_t len ) {
	size_t sz = 0;
	const void* val = ReadImpl( T_DATA, nullptr, &sz );
	ASSERT( sz == len, "buffer data read size mismatch" );
	return val;
}

const std::string Buffer::ToString() const {
	return data ? std::string( (const char*)data, lenw ) : "";
}

}
