//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef VECTOR3_H__
#define VECTOR3_H__

#include <math.h>

#include <algorithm>
#include <complex>

#include <Util/SETree.h>

#include <Math/Axis.h>
#include <Math/Vector2.h>





class GS_DllExport Vector3
{
public:
	union
	{
		struct
		{
			double x, y, z;
		};

		double v[3];
	};



	inline Vector3() : x( 0.0 ), y( 0.0 ), z( 0.0 )
	{
	}

	inline Vector3(double ix, double iy, double iz) : x( ix ), y( iy ), z( iz )
	{
	}

	inline Vector3(const Vector2 &v) : x( v.x ), y( v.y ), z( 0.0 )
	{
	}

	inline Vector3(const Vector2 &v, double iz) : x( v.x ), y( v.y ), z( iz )
	{
	}

	inline Vector3(Axis a)
	{
		switch ( a )
		{
		case AXIS_X:
			x = 1.0;
			y = 0.0;
			z = 0.0;
			break;
		case AXIS_Y:
			x = 0.0;
			y = 1.0;
			z = 0.0;
			break;
		case AXIS_Z:
			x = 0.0;
			y = 0.0;
			z = 1.0;
			break;
		case AXIS_NEGATIVE_X:
			x = -1.0;
			y = 0.0;
			z = 0.0;
			break;
		case AXIS_NEGATIVE_Y:
			x = 0.0;
			y = -1.0;
			z = 0.0;
			break;
		case AXIS_NEGATIVE_Z:
			x = 0.0;
			y = 0.0;
			z = -1.0;
			break;
		default:
			x = 0.0;
			y = 0.0;
			z = 0.0;
			break;
		}
	}



	inline bool operator==(const Vector3 &v) const
	{
		return ( x == v.x )  &&  ( y  ==  v.y )  &&  ( z == v.z );
	}

	inline bool operator!=(const Vector3 &v) const
	{
		return ( x != v.x )  ||  ( y != v.y )  ||  ( z != v.z );
	}


	inline Vector3 operator+(const Vector3 &v) const
	{
		return Vector3( x + v.x,  y + v.y,  z + v.z );
	}

	inline void operator+=(const Vector3 &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	inline Vector3 operator-(const Vector3 &v) const
	{
		return Vector3( x - v.x,  y - v.y,  z - v.z );
	}

	inline void operator-=(const Vector3 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	inline Vector3 operator*(double s) const
	{
		return Vector3( x * s,  y * s,  z * s );
	}

	inline void operator*=(double s)
	{
		x *= s;
		y *= s;
		z *= s;
	}

	inline Vector3 operator-() const
	{
		return Vector3( -x, -y, -z );
	}


	inline double dot(const Vector3 &v) const
	{
		return ( x * v.x )  +  ( y * v.y )  +  ( z * v.z );
	}

	inline Vector3 cross(const Vector3 &v) const
	{
		return Vector3( y * v.z  -  z * v.y,
							 z * v.x  -  x * v.z,
							 x * v.y  -  y * v.x );
	}

	inline Vector3 mul(const Vector3 &v) const
	{
		return Vector3( x * v.x,  y * v.y,  z * v.z );
	}

	inline double sqrLength() const
	{
		return dot( *this );
	}

	inline double length() const
	{
		return sqrt( sqrLength() );
	}

	inline double manhattanLength() const
	{
		return fabs( x ) + fabs( y ) + fabs( z );
	}

	inline void normalise()
	{
		double oneOverLength = 1.0 / length();
		x *= oneOverLength;
		y *= oneOverLength;
		z *= oneOverLength;
	}

	inline void normaliseToLength(double l)
	{
		double lOverLength = l / length();
		x *= lOverLength;
		y *= lOverLength;
		z *= lOverLength;
	}

	inline Vector3 getNormalised() const
	{
		double oneOverLength = 1.0 / length();
		return Vector3( x * oneOverLength, y * oneOverLength, z * oneOverLength );
	}

	inline Vector3 getNormalisedToLength(double l) const
	{
		double lOverLength = l / length();
		return Vector3( x * lOverLength, y * lOverLength, z * lOverLength );
	}


	inline Vector3 projectOntoUnitVector(const Vector3 &unitVector) const
	{
		return unitVector  *  dot( unitVector );
	}

	inline Vector3 projectOntoPlane(const Vector3 &unitNormal) const
	{
		return *this  -  projectOntoUnitVector( unitNormal );
	}


	inline Vector3 abs() const
	{
		return Vector3( fabs( x ), fabs( y ), fabs( z ) );
	}


	inline Axis getMajorAxis() const
	{
		double absX = fabs( x );
		double absY = fabs( y );
		double absZ = fabs( z );

		if ( absX > absY )
		{
			if ( absX > absZ )
			{
				return AXIS_X;
			}
			else
			{
				return AXIS_Z;
			}
		}
		else
		{
			if ( absY > absZ )
			{
				return AXIS_Y;
			}
			else
			{
				return AXIS_Z;
			}
		}
	}

	inline Axis getMajorSignedAxis() const
	{
		double absX = fabs( x );
		double absY = fabs( y );
		double absZ = fabs( z );

		if ( absX > absY )
		{
			if ( absX > absZ )
			{
				if ( x < 0.0 )
				{
					return AXIS_NEGATIVE_X;
				}
				else
				{
					return AXIS_X;
				}
			}
			else
			{
				if ( z < 0.0 )
				{
					return AXIS_NEGATIVE_Z;
				}
				else
				{
					return AXIS_Z;
				}
			}
		}
		else
		{
			if ( absY > absZ )
			{
				if ( y < 0.0 )
				{
					return AXIS_NEGATIVE_Y;
				}
				else
				{
					return AXIS_Y;
				}
			}
			else
			{
				if ( z < 0.0 )
				{
					return AXIS_NEGATIVE_Z;
				}
				else
				{
					return AXIS_Z;
				}
			}
		}
	}

	inline Axis getMinorAxis() const
	{
		double absX = fabs( x );
		double absY = fabs( y );
		double absZ = fabs( z );

		if ( absX < absY )
		{
			if ( absX < absZ )
			{
				return AXIS_X;
			}
			else
			{
				return AXIS_Z;
			}
		}
		else
		{
			if ( absY < absZ )
			{
				return AXIS_Y;
			}
			else
			{
				return AXIS_Z;
			}
		}
	}

	inline Axis perpendicularAxis() const
	{
		return getMinorAxis();
	}

	inline Vector3 perpendicularVector() const
	{
		return Vector3( perpendicularAxis() ).cross( *this );
	}

	inline Vector3 perpendicularTo(const Vector3 &axis) const
	{
		Vector3 u = axis.cross( *this );
		return u.cross( axis );
	}



	inline double get(Axis axis) const
	{
		switch( axis )
		{
		case AXIS_X:
			return x;
		case AXIS_NEGATIVE_X:
			return -x;
		case AXIS_Y:
			return y;
		case AXIS_NEGATIVE_Y:
			return -y;
		case AXIS_Z:
			return z;
		case AXIS_NEGATIVE_Z:
			return -z;
		default:
			return 0.0;
		}
	}

	inline double getAbs(Axis axis) const
	{
		switch( axis )
		{
		case AXIS_NEGATIVE_X:
		case AXIS_X:
			return x;
		case AXIS_NEGATIVE_Y:
		case AXIS_Y:
			return y;
		case AXIS_NEGATIVE_Z:
		case AXIS_Z:
			return z;
		default:
			return 0.0;
		}
	}


	inline void set(Axis axis, double v)
	{
		switch( axis )
		{
		case AXIS_X:
			x = v;
			break;
		case AXIS_NEGATIVE_X:
			x = -v;
			break;
		case AXIS_Y:
			y = v;
			break;
		case AXIS_NEGATIVE_Y:
			y = -v;
			break;
		case AXIS_Z:
			z = v;
			break;
		case AXIS_NEGATIVE_Z:
			z = -v;
			break;
		default:
			break;
		}
	}

	inline void setAbs(Axis axis, double v)
	{
		switch( axis )
		{
		case AXIS_NEGATIVE_X:
		case AXIS_X:
			x = v;
			break;
		case AXIS_NEGATIVE_Y:
		case AXIS_Y:
			y = v;
			break;
		case AXIS_NEGATIVE_Z:
		case AXIS_Z:
			z = v;
			break;
		default:
			break;
		}
	}


	inline void flatten(Axis axis)
	{
		set( axis, 0.0 );
	}




	inline double argPolar(Axis axis)
	{
		switch( axis )
		{
		case AXIS_X:
			return std::arg( std::complex<double>( -z, y ) );
		case AXIS_NEGATIVE_X:
			return -std::arg( std::complex<double>( -z, y ) );
		case AXIS_Y:
			return std::arg( std::complex<double>( x, -z ) );
		case AXIS_NEGATIVE_Y:
			return -std::arg( std::complex<double>( x, -z ) );
		case AXIS_Z:
			return std::arg( std::complex<double>( x, y ) );
		case AXIS_NEGATIVE_Z:
			return -std::arg( std::complex<double>( x, y ) );
		default:
			return 0.0;
		}
	}


	inline Vector2 toVector2() const
	{
		return Vector2( x, y );
	}

	inline Vector2 toVector2Axis(Axis axis) const
	{
		switch( axis )
		{
		case AXIS_X:
			return Vector2( -z, y );
		case AXIS_NEGATIVE_X:
			return Vector2( z, y );
		case AXIS_Y:
			return Vector2( x, -z );
		case AXIS_NEGATIVE_Y:
			return Vector2( x, z );
		case AXIS_Z:
			return Vector2( x, y );
		case AXIS_NEGATIVE_Z:
			return Vector2( -x, y );
		default:
			return Vector2( x, y );
		}
	}


	static Vector3 min(const Vector3 &a, const Vector3 &b)
	{
		return Vector3( std::min( a.x, b.x ), std::min( a.y, b.y ), std::min( a.z, b.z ) );
	}

	static Vector3 max(const Vector3 &a, const Vector3 &b)
	{
		return Vector3( std::max( a.x, b.x ), std::max( a.y, b.y ), std::max( a.z, b.z ) );
	}



	inline void read(FILE *f)
	{
		fread( &x, sizeof(double), 1, f );
		fread( &y, sizeof(double), 1, f );
		fread( &z, sizeof(double), 1, f );
	}

	inline void write(FILE *f) const
	{
		fwrite( &x, sizeof(double), 1, f );
		fwrite( &y, sizeof(double), 1, f );
		fwrite( &z, sizeof(double), 1, f );
	}

	inline void read(SENode &node)
	{
		node[0]  >>  x;
		node[1]  >>  y;
		node[2]  >>  z;
	}

	inline void write(SENode &node) const
	{
		node.addItem()  <<  x;
		node.addItem()  <<  y;
		node.addItem()  <<  z;
	}
};





#endif
