//---------------------------------------------------
// Name: jbsCommon : Vector.h
// Desc:  Deals with basic vector operations
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _JBSCOMMON_MATH_VECTOR_H_
#define _JBSCOMMON_MATH_VECTOR_H_

#include <algorithm>
#include "Types.h"

#ifndef _WIN32
// This is actually here to correct for
// a bug in the MSVC STL.
using std::min;
using std::max;
#endif

#include <iostream>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

namespace jbsCommon
{

//==========[ Forward References ]=========================

template <class T> class Vec;
template <class T> class Vec3;
template <class T> class Vec4;
template <class T> class Mat3;
template <class T> class Mat4;

//==========[ Exception Classes ]==========================

class VectorSizeMismatch {};

//==========[ class Vec2 ]=================================

template <class T>
class Vec2 {

	//---[ Private Variable Declarations ]-------

public:

	union
	{
		T	   n[2]; 
		struct { T x,y; };
		struct { T u,v; };
	};

public:

	//---[ Constructors ]------------------------

	Vec2() 
	{ 
		n[0] = (T)0; 
		n[1] = (T)0; 
	}

	Vec2( const T x, const T y )
	{ 
		n[0] = x; 
		n[1] = y; 
	}

	Vec2( const Vec2<T>& v )
	{ 
		n[0] = v.n[0]; 
		n[1] = v.n[1]; 
	}

	//---[ Equal Operators ]---------------------

	Vec2<T>& operator=( const Vec2<T>& v )
	{ 
		n[0] = v.n[0]; 
		n[1] = v.n[1]; 
		return *this; 
	}

	Vec2<T>& operator +=( const Vec2<T>& v )
	{ 
		n[0] += v.n[0]; 
		n[1] += v.n[1]; 
		return *this; 
	}

	Vec2<T>& operator -= ( const Vec2<T>& v )
	{ 
		n[0] -= v.n[0]; 
		n[1] -= v.n[1]; 
		return *this; 
	}

	Vec2<T>& operator *= ( const T d )
	{ 
		n[0] *= d; 
		n[1] *= d; 
		return *this; 
	}

	Vec2<T>& operator /= ( const T d )
	{ 
		n[0] /= d; 
		n[1] /= d; 
		return *this; 
	}

	//---[ Access Operators ]--------------------

	T& operator []( int i )
	{ 
		return n[i]; 
	}

	T operator []( int i ) const 
	{ 
		return n[i]; 
	}

	//---[ Arithmetic Operators ]----------------

	Vec2<T> operator-( const Vec2<T>& a ) { return Vec2<T>(n[0]-a.n[0],n[1]-a.n[1]); }
	Vec2<T> operator+( const Vec2<T>& a ) { return Vec2<T>(a.n[0]+n[0],a.n[1]+n[1]); }
	bool operator == ( const Vec2<T>& a ) { return fequal(a.n[0],n[0]) && fequal(a.n[1],n[1]); }

	//---[ Conversion Operators ]----------------

	const T* getPointer() const { return n; }

	//---[ Length Methods ]----------------------

	double length2() const
	{ 
		return n[0]*n[0] + n[1]*n[1]; 
	}

	double length() const
	{ 
		return sqrt( length2() ); 
	}

	//---[ Normalization ]-----------------------

	void normalize() 
	{ 
		double len = length();
		n[0] /= len; n[1] /= len;
	}

	//---[ Zero Test ]---------------------------

	bool iszero() { return ( (n[0]==0 && n[1]==0) ? true : false); };
	void zeroElements() { memset(n,0,sizeof(T)*2); }	
};

typedef Vec2<int> Vec2i;
typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;

//==========[ class Vec3 ]=================================

template <class T>
class Vec3 
{	

public:
		
	union
	{
		T	   n[3]; 
		struct { T x,y,z; };
		struct { T r,g,b; };
	};

	// Constructor

	Vec3()
	{ 
		n[0] = 0.0; 
		n[1] = 0.0; 
		n[2] = 0.0; 
	}

	Vec3( const T tx, const T ty, const T tz )
	{ 
		n[0] = tx; 
		n[1] = ty; 
		n[2] = tz; 
	}

	Vec3( const Vec3<T>& v )
	{ 
		n[0] = v.n[0]; 
		n[1] = v.n[1]; 
		n[2] = v.n[2]; 
	}

	Vec3( T homoVal )
	{
		n[0] = homoVal;
		n[1] = homoVal;
		n[2] = homoVal;
	}
	
	Vec3( const Vec4<T>& v )
	{ 
		n[0] = v[0]; 
		n[1] = v[1]; 
		n[2] = v[2]; 
	}

	//Math Overloads

	Vec3<T>& operator=( const Vec3<T>& v )
	{ 
		n[0] = v.n[0]; 
		n[1] = v.n[1]; 
		n[2] = v.n[2]; 
		return *this; 
	}

	Vec3<T>& operator +=( const Vec3<T>& v )
	{ 
		n[0] += v.n[0]; 
		n[1] += v.n[1]; 
		n[2] += v.n[2]; 
		return *this; 
	}

	Vec3<T>& operator -= ( const Vec3<T>& v )
	{ 
		n[0] -= v.n[0]; 
		n[1] -= v.n[1]; 
		n[2] -= v.n[2]; 
		return *this; 
	}

	Vec3<T>& operator *= ( const T d )
	{ 
		n[0] *= d; 
		n[1] *= d; 
		n[2] *= d; 
		return *this; 
	}

	Vec3<T>& operator /= ( const T d )
	{ 
		n[0] /= d; 
		n[1] /= d; 
		n[2] /= d; 
		return *this; 
	}

	bool operator == ( const Vec3<T>& b )
	{
		return fequal( n[0], b.n[0] ) &&
			   fequal( n[1], b.n[1] ) &&
			   fequal( n[2], b.n[2] );
	}

	bool operator != ( const Vec3<T>& b )
	{
		return !(*this == b);
	}

	// Access Operators 

	T& operator []( int i )
	{ 
		return n[i]; 
	}

	T operator []( int i ) const 
	{ 
		return n[i]; 
	}

	// Arithmetic Operators 

	Vec3<T> operator-( const Vec3<T>& a ) const 
	{ 
		return Vec3<T>( n[0]-a.n[0], n[1]-a.n[1], n[2]-a.n[2] ); 
	}

	Vec3<T> operator+( const Vec3<T>& a ) const 
	{ 
		return Vec3<T>( a.n[0]+n[0], a.n[1]+n[1], a.n[2]+n[2] ); 
	}

	// Conversion Operators 

	const T* getPointer() const 
	{ 
		return n; 
	}

	// Length Methods 

	double length2() const
	{ 
		return n[0]*n[0] + n[1]*n[1] + n[2]*n[2]; 
	}

	double length() const
	{ 
		return (double)QuickSquareRoot( (float)length2() );
	}

	double Magnitude2() const
	{
		return length2();
	}

	double Magnitude() const
	{
		return length();
	}

	static float InvSquareRoot( float val )
	{		
		float half = 0.5f * val;
		int i = *(int*)&val; 
		i = 0x5f3759df - (i>>1); 
		val = *(float*)&i; 
		val = val*(1.5f-half*val*val); 
		
		return val;
	}

	static float QuickSquareRoot( float val )
	{		
		return val * InvSquareRoot(val);
	}	

	// Normalization

	void normalize() 
	{ 
		double len = length();
		n[0] /= (T)len; 
		n[1] /= (T)len; 
		n[2] /= (T)len;
	}

	void clamp() 
	{
		int i;
		for (i=0; i < 3; i++) 
		{
			if (n[i] < 0) n[i] = 0.0;
			if (n[i] > 1) n[i] = 1.0;
		}
	}

	Vec3<T> Reflect( const Vec3<T>& normal )
	{
		Vec3<T> V = *this;
		double lengthN = normal.length();
		double lengthV = V.length();

		assert( lengthN != 0.0f && lengthV != 0.0f && "Vec : Reflect : 0 Length" );

		return V - 2 * ( V * normal / (lengthN * lengthV) ) * normal;
	}

	static Vec3<T> ZeroVector()
	{
		return Vec3<T>( 0 );
	}

	static Vec3<T> UnityVector()
	{
		return Vec3<T>( 1 );
	}

	Vec3<T> Normalized() const
	{
		float fLength = (float)length();
		if( fLength > 0 )
		{
			return *this * ( 1.0f / fLength );
		}
		else
		{
			return Vec3<T>::ZeroVector();
		}
	}			

	//---[ Zero Test ]---------------------------

	bool IsZero() 
	{ 
		return ( (n[0]==0 && n[1]==0 && n[2]==0) ? true : false); 
	};

	void ZeroElements() 
	{ 
		memset(n,0,sizeof(T)*3); 
	}	

	// Friend Methods 

#ifdef WIN32
	// VCC is non-standard
	template <class U> friend T operator *( const Vec3<T>& a, const Vec4<T>& b );
	template <class U> friend T operator *( const Vec4<T>& b, const Vec3<T>& a );
//	template <class U> friend Vec3<T> operator -( const Vec3<T>& v );
	template <class U> friend Vec3<T> operator *( const Vec3<T>& a, const double d );
	template <class U> friend Vec3<T> operator *( const double d, const Vec3<T>& a );
	template <class U> friend Vec3<T> operator *( const Vec3<T>& v, Mat4<T>& a );
	template <class U> friend T operator *( const Vec3<T>& a, const Vec3<T>& b );
	template <class U> friend Vec3<T> operator *( const Mat3<T>& a, const Vec3<T>& v );
	template <class U> friend Vec3<T> operator *( const Vec3<T>& v, const Mat3<T>& a );
	template <class U> friend Vec3<T> operator *( const Mat4<T>& a, const Vec3<T>& v );
	template <class U> friend Vec3<T> operator /( const Vec3<T>& a, const double d );
	template <class U> friend Vec3<T> operator ^( const Vec3<T>& a, const Vec3<T>& b );
	template <class U> friend bool operator ==( const Vec3<T>& a, const Vec3<T>& b );
	template <class U> friend bool operator !=( const Vec3<T>& a, const Vec3<T>& b );
	template <class U> friend std::ostream& operator <<( std::ostream& os, const Vec3<T>& v );
	template <class U> friend std::istream& operator >>( std::istream& is, Vec3<T>& v );
	template <class U> friend Vec3<T> minimum( const Vec3<T>& a, const Vec3<T>& b );
	template <class U> friend Vec3<T> maximum( const Vec3<T>& a, const Vec3<T>& b );
	template <class U> friend Vec3<T> prod( const Vec3<T>& a, const Vec3<T>& b );
#else
	friend T operator * <>( const Vec3<T>& a, const Vec4<T>& b );
	friend T operator * <>( const Vec4<T>& b, const Vec3<T>& a );
//	friend Vec3<T> operator - <>( const Vec3<T>& v );
	friend Vec3<T> operator * <>( const Vec3<T>& a, const double d );
	friend Vec3<T> operator * <>( const double d, const Vec3<T>& a );
	friend Vec3<T> operator * <>( const Vec3<T>& v, Mat4<T>& a );
	friend T operator * <>( const Vec3<T>& a, const Vec3<T>& b );
	friend Vec3<T> operator * <>( const Mat3<T>& a, const Vec3<T>& v );
	friend Vec3<T> operator * <>( const Vec3<T>& v, const Mat3<T>& a );
	friend Vec3<T> operator * <>( const Mat4<T>& a, const Vec3<T>& v );
	friend Vec3<T> operator / <>( const Vec3<T>& a, const double d );
	friend Vec3<T> operator ^ <>( const Vec3<T>& a, const Vec3<T>& b );
	friend bool operator == <>( const Vec3<T>& a, const Vec3<T>& b );
	friend bool operator != <>( const Vec3<T>& a, const Vec3<T>& b );
	friend std::ostream& operator << <>( std::ostream& os, const Vec3<T>& v );
	friend std::istream& operator >> <>( std::istream& is, Vec3<T>& v );
	friend Vec3<T> minimum<>( const Vec3<T>& a, const Vec3<T>& b );
	friend Vec3<T> maximum<>( const Vec3<T>& a, const Vec3<T>& b );
	friend Vec3<T> prod<>( const Vec3<T>& a, const Vec3<T>& b );
#endif
};

typedef Vec3<int> Vec3i;
typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;


//==========[ class Vec4 ]=================================

template <class T>
class Vec4 
{

public:

	union
	{
		T		n[4];
		struct { T x,y,z,w; };
		struct { T r,g,b,a; };
	};

public:
	
	// Constructors 

	Vec4() 
	{ 
		n[0] = 0.0; 
		n[1] = 0.0; 
		n[2] = 0.0; 
		n[3] = 0.0; 
	}

	Vec4( const T tx, const T ty, const T tz, const T tw )
	{ 
		n[0] = tx; 
		n[1] = ty; 
		n[2] = tz; 
		n[3] = tw; 
	}

	Vec4( const T homoVal )
	{
		n[0] = homoVal;
		n[1] = homoVal;
		n[2] = homoVal;
		n[3] = homoVal;
	}

	Vec4( const Vec4& v )
	{ 
		n[0] = v.n[0]; 
		n[1] = v.n[1]; 
		n[2] = v.n[2]; 
		n[3] = v.n[3]; 
	}

	// Equal Operators 

	Vec4<T>& operator =( const Vec4<T>& v )
	{ 
		n[0] = v.n[0]; 
		n[1] = v.n[1]; 
		n[2] = v.n[2]; 
		n[3] = v.n[3];
		 return *this; 
	}

	Vec4<T>& operator +=( const Vec4<T>& v )
	{ 
		n[0] += v.n[0]; 
		n[1] += v.n[1]; 
		n[2] += v.n[2]; 
		n[3] += v.n[3];
		return *this; 
	}

	Vec4<T>& operator -= ( const Vec4<T>& v )
	{ 
		n[0] -= v.n[0]; 
		n[1] -= v.n[1]; 
		n[2] -= v.n[2]; 
		n[3] -= v.n[3];
		return *this; 
	}

	Vec4<T>& operator *= ( const T d )
	{ 
		n[0] *= d; 
		n[1] *= d; 
		n[2] *= d; 
		n[3] *= d; 
		return *this; 
	}

	Vec4<T>& operator /= ( const T d )
	{ 
		n[0] /= d; 
		n[1] /= d; 
		n[2] /= d; 
		n[3] /= d; 
		return *this; 
	}

	bool operator == ( const Vec4<T>& b )
	{
		return fequal( n[0], b.n[0] ) &&
			   fequal( n[1], b.n[1] ) &&
			   fequal( n[2], b.n[2] ) &&
			   fequal( n[3], b.n[3] );
	}

	bool operator != ( const Vec4<T>& b )
	{
		return !(*this == b);
	}

	// Access Operators 
	T& operator []( int i )
	{ 
		return n[i]; 
	}

	T operator []( int i ) const 
	{ 
		return n[i]; 
	}

	// Arithmetic Operators 

	Vec4<T> operator-( const Vec4<T>& a ) 
	{ 
		return Vec4<T>(n[0]-a.n[0],n[1]-a.n[1],n[2]-a.n[2],n[3]-a.n[3]); 
	}

	Vec4<T> operator+( const Vec4<T>& a ) 
	{ 
		return Vec4<T>(a.n[0]+n[0],a.n[1]+n[1],a.n[2]+n[2],a.n[3]+n[3]); 
	}

	// Length Methods 

	double length2() const
	{ 
		return n[0]*n[0] + n[1]*n[1] + n[2]*n[2] + n[3]*n[3]; 
	}

	double length() const
	{ 
		//return (double)Vec3<T>::QuickSquareRoot( (float)length2() );
		return sqrt( length2() );
	}	

	// Zero Test 

	bool IsZero() const 
	{ 
		return n[0]==0&&n[1]==0&&n[2]==0&&n[3]==0; 
	}

	void ZeroElements() 
	{ 
		memset(n,0,4*sizeof(T)); 
	}

	// Normalization 

	void Normalize() 
	{
		double len = length();
		n[0] /= len; 
		n[1] /= len; 
		n[2] /= len; 
		n[3] /= len;
	}

	Vec4<T> Normalized() const
	{
		double dLength = length();
		if( dLength > 0 )
			return *this * (1.0 / dLength);
		else
			return Vec4<T>::ZeroVector();
	}

	static Vec4<T> ZeroVector()
	{
		return Vec4<T>(0);
	}

	static Vec4<T> UnityVector()
	{
		return Vec4<T>(1);
	}

	Vec3<T> ToVec3() const
	{
		return *(Vec3<T>*)this;
	}
	
	// Friend Methods 

#ifdef WIN32
	// VCC is non-standard
	template <class U> friend T operator *( const Vec3<T>& a, const Vec4<T>& b );
	template <class U> friend T operator *( const Vec4<T>& b, const Vec3<T>& a );
//	template <class U> friend Vec4<T> operator -( const Vec4<T>& v );
	template <class U> friend Vec4<T> operator *( const Vec4<T>& a, const double d );
	template <class U> friend Vec4<T> operator *( const double d, const Vec4<T>& a );
	template <class U> friend T operator *( const Vec4<T>& a, const Vec4<T>& b );
	template <class U> friend Vec4<T> operator *( const Mat4<T>& a, const Vec4<T>& v );
//	template <class U> friend Vec4<T> operator *( const Vec4<T>& v, const Mat4<T>& a );
	template <class U> friend Vec4<T> operator /( const Vec4<T>& a, const double d );
//	template <class U> friend Vec4<T> operator ^( const Vec4<T>& a, const Vec4<T>& b );
	template <class U> friend bool operator ==( const Vec4<T>& a, const Vec4<T>& b );
	template <class U> friend bool operator !=( const Vec4<T>& a, const Vec4<T>& b );
	template <class U> friend std::ostream& operator <<( std::ostream& os, const Vec4<T>& v );
	template <class U> friend std::istream& operator >>( std::istream& is, Vec4<T>& v );
	template <class U> friend Vec4<T> minimum( const Vec4<T>& a, const Vec4<T>& b );
	template <class U> friend Vec4<T> maximum( const Vec4<T>& a, const Vec4<T>& b );
	template <class U> friend Vec4<T> prod( const Vec4<T>& a, const Vec4<T>& b );
#else
	friend T operator * <>( const Vec3<T>& a, const Vec4<T>& b );
	friend T operator * <>( const Vec4<T>& b, const Vec3<T>& a );
//	friend Vec4<T> operator - <>( const Vec4<T>& v );
	friend Vec4<T> operator * <>( const Vec4<T>& a, const double d );
	friend Vec4<T> operator * <>( const double d, const Vec4<T>& a );
	friend T operator * <>( const Vec4<T>& a, const Vec4<T>& b );
	friend Vec4<T> operator * <>( const Mat4<T>& a, const Vec4<T>& v );
//	friend Vec4<T> operator * <>( const Vec4<T>& v, const Mat4<T>& a );
	friend Vec4<T> operator / <>( const Vec4<T>& a, const double d );
//	friend Vec4<T> operator ^ <>( const Vec4<T>& a, const Vec4<T>& b );
	friend bool operator == <>( const Vec4<T>& a, const Vec4<T>& b );
	friend bool operator != <>( const Vec4<T>& a, const Vec4<T>& b );
	friend std::ostream& operator << <>( std::ostream& os, const Vec4<T>& v );
	friend std::istream& operator >> <>( std::istream& is, Vec4<T>& v );
	friend Vec4<T> minimum <>( const Vec4<T>& a, const Vec4<T>& b );
	friend Vec4<T> maximum <>( const Vec4<T>& a, const Vec4<T>& b );
	friend Vec4<T> prod <>( const Vec4<T>& a, const Vec4<T>& b );
#endif
};

typedef Vec4<int> Vec4i;
typedef Vec4<float> Vec4f;
typedef Vec4<double> Vec4d;

//==========[ Inline Method Definitions (Vectors) ]========

template <class T>
inline T operator *( const Vec3<T>& a, const Vec4<T>& b ) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + b[3];
}

template <class T>
inline T operator *( const Vec4<T>& b, const Vec3<T>& a ) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + b[3];
}

template <class T>
inline Vec3<T> operator -(const Vec3<T>& v) {
	return Vec3<T>( -v.n[0], -v.n[1], -v.n[2] );
}

template <class T>
inline Vec3<T> operator *(const Vec3<T>& a, const double d ) 
{
	return Vec3<T>( (T)(a.n[0] * d), 
					(T)(a.n[1] * d), 
					(T)(a.n[2] * d) );
}

template <class T>
inline Vec3<T> operator *(const double d, const Vec3<T>& a) {
	return a * d;
}

template <class T>
inline Vec3<T> operator *(const Mat4<T>& a, const Vec3<T>& v){
	return Vec3<T>( a.n[0]*v.n[0]+a.n[1]*v.n[1]+a.n[2]*v.n[2]+a.n[3],
					a.n[4]*v.n[0]+a.n[5]*v.n[1]+a.n[6]*v.n[2]+a.n[7],
					a.n[8]*v.n[0]+a.n[9]*v.n[1]+a.n[10]*v.n[2]+a.n[11] );
}

template <class T>
inline Vec3<T> operator *(const Vec3<T>& v, Mat4<T>& a) {
	return a.transpose() * v;
}

template <class T>
inline T operator *(const Vec3<T>& a, const Vec3<T>& b){
	return a.n[0]*b.n[0] + a.n[1]*b.n[1] + a.n[2]*b.n[2];
}

template <class T>
inline Vec3<T> operator *( const Mat3<T>& a, const Vec3<T>& v ) {
	return Vec3<T>( a.n[0]*v.n[0]+a.n[1]*v.n[1]+a.n[2]*v.n[2],
					a.n[3]*v.n[0]+a.n[4]*v.n[1]+a.n[5]*v.n[2],
					a.n[6]*v.n[0]+a.n[7]*v.n[1]+a.n[8]*v.n[2] );
}

template <class T>
inline Vec3<T> operator *( const Vec3<T>& v, const Mat3<T>& a ) {
	return a.transpose() * v;
}

template <class T>
inline Vec3<T> operator /(const Vec3<T>& a, const double d){
	return Vec3<T>( a.n[0] / d, a.n[1] / d, a.n[2] / d );
}

template <class T>
inline Vec3<T> operator ^(const Vec3<T>& a, const Vec3<T>& b) {
	return Vec3<T>( a.n[1]*b.n[2] - a.n[2]*b.n[1],
					a.n[2]*b.n[0] - a.n[0]*b.n[2],
					a.n[0]*b.n[1] - a.n[1]*b.n[0] );
}

template <class T>
inline bool operator ==(const Vec3<T>& a, const Vec3<T>& b) {
	return a.n[0]==b.n[0] && a.n[1] == b.n[1] && a.n[2] == b.n[2];
}

template <class T>
inline bool operator !=(const Vec3<T>& a, const Vec3<T>& b) {
	return !( a == b );
}

template <class T>
inline std::ostream& operator <<( std::ostream& os, const Vec3<T>& v ) {
	return os << v[0] << " " << v[1] << " " << v[2];
}

template <class T>
inline std::istream& operator >>( std::istream& is, Vec3<T>& v ) {
	return is >> v.n[0] >> v.n[1] >> v.n[2];
}

template <class T>
inline Vec3<T> minimum( const Vec3<T>& a, const Vec3<T>& b ) {
	return Vec3<T>( min(a.n[0],b.n[0]), min(a.n[1],b.n[1]), min(a.n[2],b.n[2]) );
}

template <class T>
inline Vec3<T> maximum(const Vec3<T>& a, const Vec3<T>& b) {
	return Vec3<T>( max(a.n[0],b.n[0]), max(a.n[1],b.n[1]), max(a.n[2],b.n[2]) );
}

template <class T>
inline Vec3<T> prod(const Vec3<T>& a, const Vec3<T>& b ) {
	return Vec3<T>( a.n[0]*b.n[0], a.n[1]*b.n[1], a.n[2]*b.n[2] );
}

template <class T>
inline Vec4<T> operator -( const Vec4<T>& v ) {
	return Vec4<T>( -v.n[0], -v.n[1], -v.n[2], -v.n[3] );
}

template <class T>
inline Vec4<T> operator *(const Vec4<T>& a, const double d ) {
	return Vec4<T>( a.n[0] * d, a.n[1] * d, a.n[2] * d, a.n[3] * d );
}

template <class T>
inline Vec4<T> operator *(const double d, const Vec4<T>& a) {
	return a * d;
}

template <class T>
inline T operator *(const Vec4<T>& a, const Vec4<T>& b) {
	return a.n[0]*b.n[0] + a.n[1]*b.n[1] + a.n[2]*b.n[2] + a.n[3]*b.n[3];
}

template <class T>
inline Vec4<T> operator *(const Mat4<T>& a, const Vec4<T>& v) {
	return Vec4<T>( a.n[0]*v.n[0]+a.n[1]*v.n[1]+a.n[2]*v.n[2]+a.n[3]*v.n[3],
					a.n[4]*v.n[0]+a.n[5]*v.n[1]+a.n[6]*v.n[2]+a.n[7]*v.n[3],
					a.n[8]*v.n[0]+a.n[9]*v.n[1]+a.n[10]*v.n[2]+a.n[11]*v.n[3],
					a.n[12]*v.n[0]+a.n[13]*v.n[1]+a.n[14]*v.n[2]+a.n[15]*v.n[3]);
}

template <class T>
inline Vec4<T> operator *( const Vec4<T>& v, Mat4<T>& a ){
	return a.transpose() * v;
}

template <class T>
inline Vec4<T> operator /(const Vec4<T>& a, const double d) {
	return Vec4<T>( a.n[0] / d, a.n[1] / d, a.n[2] / d, a.n[3] / d );
}

template <class T>
inline bool operator ==(const Vec4<T>& a, const Vec4<T>& b) {
	return a.n[0] == b.n[0] && a.n[1] == b.n[1] && a.n[2] == b.n[2] 
	    && a.n[3] == b.n[3];
}

template <class T>
inline bool operator !=(const Vec4<T>& a, const Vec4<T>& b) {
	return !( a == b );
}

template <class T>
inline std::ostream& operator <<( std::ostream& os, const Vec4<T>& v ) {
	return os << v.n[0] << " " << v.n[1] << " " << v.n[2] << " " << v.n[3];
}

template <class T>
inline std::istream& operator >>( std::istream& is, Vec4<T>& v ) {
	return is >> v.n[0] >> v.n[1] >> v.n[2] >> v.n[3];
}

template <class T>
inline Vec4<T> minimum( const Vec4<T>& a, const Vec4<T>& b ) {
	return Vec4<T>( min(a.n[0],b.n[0]), min(a.n[1],b.n[1]), min(a.n[2],b.n[2]),
	             min(a.n[3],b.n[3]) );
}

template <class T>
inline Vec4<T> maximum( const Vec4<T>& a, const Vec4<T>& b) {
	return Vec4<T>( max(a.n[0],b.n[0]), max(a.n[1],b.n[1]), max(a.n[2],b.n[2]),
	             max(a.n[3],b.n[3]) );
}

template <class T>
inline Vec4<T> prod(const Vec4<T>& a, const Vec4<T>& b ) {
	return Vec4<T>( a.n[0]*b.n[0], a.n[1]*b.n[1], a.n[2]*b.n[2], a.n[3]*b.n[3] );
}

template <class T>
Vec3<T> vec4to3(Vec4<T> &v) {
	return Vec3<T>(v[0], v[1], v[2]);
}

}; //end jbsCommon


#endif //end _JBSCOMMON_MATH_VECTOR_H_

