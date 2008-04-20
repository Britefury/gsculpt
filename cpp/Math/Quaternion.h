//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************

//
// Quaternion <-> Euler angle conversion code:
// Used:
//			http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
//	as a reference.
//
#ifndef QUATERNION_H__
#define QUATERNION_H__

#include <math.h>

#include <complex>

#include <Math/MathConstants.h>
#include <Math/epsilon.h>
#include <Math/Vector3.h>
#include <Math/Axes3.h>
#include <Math/EulerRotation.h>
#include <Math/clamp.h>



class GS_DllExport Quaternion
{
public:
	double x, y, z, w;


	inline Quaternion() : x( 0.0 ), y( 0.0 ), z( 0.0 ), w( 1.0 )
	{
	}

	inline Quaternion(double ix, double iy , double iz, double iw)
							: x( ix ), y( iy ), z( iz ), w( iw )
	{
	}



	inline Quaternion operator+(const Quaternion& q) const
	{
		return Quaternion( x + q.x,  y + q.y,  z + q.z,  w + q.w );
	}

	inline void operator+=(const Quaternion& q)
	{
		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;
	}


	inline Quaternion operator-(const Quaternion& q) const
	{
		return Quaternion( x - q.x,  y - q.y,  z - q.z,  w - q.w );
	}

	inline void operator-=(const Quaternion& q)
	{
		x -= q.x;
		y -= q.y;
		z -= q.z;
		w -= q.w;
	}


	inline Quaternion operator*(const Quaternion& q) const
	{
		return Quaternion( ( w * q.x ) + ( x * q.w ) + ( y * q.z ) - ( z * q.y ),
							( w * q.y ) - ( x * q.z ) + ( y * q.w ) + ( z * q.x ),
							( w * q.z ) + ( x * q.y ) - ( y * q.x ) + ( z * q.w ),
							( w * q.w ) - ( x * q.x ) - ( y * q.y ) - ( z * q.z ) );
	}

	inline void operator*=(const Quaternion& q)
	{
		double tempW = ( w * q.w ) - ( x * q.x ) - ( y * q.y ) - ( z * q.z );
		double tempX = ( w * q.x ) + ( x * q.w ) + ( y * q.z ) - ( z * q.y );
		double tempY = ( w * q.y ) - ( x * q.z ) + ( y * q.w ) + ( z * q.x );
		z			 = ( w * q.z ) + ( x * q.y ) - ( y * q.x ) + ( z * q.w );
		w = tempW;
		x = tempX;
		y = tempY;
	}


	inline Quaternion operator/(const Quaternion& q) const
	{
		return q.inverse()  *  *this;
	}

	inline void operator/=(const Quaternion& q)
	{
		*this = q.inverse()  *  *this;
	}



	inline double norm() const
	{
		return ( x * x )  +  ( y * y )  +  ( z * z )  +  ( w * w );
	}

	inline double modulus() const
	{
		return sqrt( norm() );
	}

	inline Quaternion conjugate() const
	{
		return Quaternion( w, -x, -y, -z );
	}

	inline Quaternion inverse() const
	{
		double normRecip = 1.0 / norm();

		return Quaternion( -x * normRecip,  -y * normRecip,  -z * normRecip,  w * normRecip );
	}


	inline EulerRotation toEuler() const
	{
 		EulerRotation euler;

		double sqw = w*w;
		double sqx = x*x;
		double sqy = y*y;
		double sqz = z*z;

		double length = sqx + sqy + sqz + sqw;
		double alpha = x*y + z*w;

		if ( alpha  >  length * ( 0.5 - EPSILON ) )
		{
			// singularity at north pole
			euler.y = 2.0 * atan2( x, w );
			euler.z = M_PI / 2.0;
			euler.x = 0.0;
		}
		else if (alpha  <  -length * ( 0.5 - EPSILON ))
		{
			// singularity at south pole
			euler.y = -2.0 * atan2( x, w );
			euler.z = -M_PI / 2.0;
			euler.x = 0.0;
		}
		else
		{
			euler.y = atan2( 2.0 * y * w  -  2.0 * x * z , sqx - sqy - sqz + sqw);
			euler.z = asin( 2.0*alpha / length);
			euler.x = atan2( 2.0 * x * w  -  2.0 * y * z , -sqx + sqy - sqz + sqw);
		}

		return euler;
	}


	//rotation
	inline static Quaternion rotateX(double sinHalfTheta, double cosHalfTheta)
	{
		return Quaternion( sinHalfTheta, 0.0, 0.0,  cosHalfTheta );
	}

	inline static Quaternion rotateY(double sinHalfTheta, double cosHalfTheta)
	{
		return Quaternion( 0.0, sinHalfTheta, 0.0,  cosHalfTheta );
	}

	inline static Quaternion rotateZ(double sinHalfTheta, double cosHalfTheta)
	{
		return Quaternion( 0.0, 0.0, sinHalfTheta,  cosHalfTheta );
	}

	inline static Quaternion rotateX(double angle)
	{
		angle *= 0.5;
		return rotateX( sin( angle ), cos( angle ) );
	}

	inline static Quaternion rotateY(double angle)
	{
		angle *= 0.5;
		return rotateY( sin( angle ), cos( angle ) );
	}

	inline static Quaternion rotateZ(double angle)
	{
		angle *= 0.5;
		return rotateZ( sin( angle ), cos( angle ) );
	}


	inline static Quaternion py_rotateXSinCos(double sinHalfTheta, double cosHalfTheta)
	{
		return rotateX( sinHalfTheta, cosHalfTheta );
	}

	inline static Quaternion py_rotateYSinCos(double sinHalfTheta, double cosHalfTheta)
	{
		return rotateY( sinHalfTheta, cosHalfTheta );
	}

	inline static Quaternion py_rotateZSinCos(double sinHalfTheta, double cosHalfTheta)
	{
		return rotateZ( sinHalfTheta, cosHalfTheta );
	}

	inline static Quaternion py_rotateX(double angle)
	{
		return rotateX( angle );
	}

	inline static Quaternion py_rotateY(double angle)
	{
		return rotateY( angle );
	}

	inline static Quaternion py_rotateZ(double angle)
	{
		return rotateZ( angle );
	}




	inline static Quaternion rotate(const Vector3 &axis, double sinHalfTheta, double cosHalfTheta)
	{
		return Quaternion( axis.x * sinHalfTheta, axis.y * sinHalfTheta, axis.z * sinHalfTheta,  cosHalfTheta );
	}

	inline static Quaternion py_rotateAxisSinCos(const Vector3 &axis, double sinHalfTheta, double cosHalfTheta)
	{
		return rotate( axis, sinHalfTheta, cosHalfTheta );
	}

	inline static Quaternion rotate(const Vector3 &axis, double angle)
	{
		angle *= 0.5;
		return rotate( axis, sin( angle ), cos( angle ) );
	}

	inline static Quaternion py_rotateAxis(const Vector3 &axis, double angle)
	{
		return rotate( axis, angle );
	}

	inline static Quaternion rotate(const EulerRotation &euler)
	{
		double sx = sin(euler.x / 2);
		double cx = cos(euler.x / 2);
		double sy = sin(euler.y / 2);
		double cy = cos(euler.y / 2);
		double sz = sin(euler.z / 2);
		double cz = cos(euler.z / 2);

		Quaternion q;
		q.w = cy * cz * cx  -  sy * sz * sx;
		q.x = sy * sz * cx  +  cy * cz * sx;
		q.y = sy * cz * cx  +  cy * sz * sx;
		q.z = cy * sz * cx  -  sy * cz * sx;

		return q;
	}

	inline static Quaternion py_rotateEuler(const EulerRotation &euler)
	{
		return rotate( euler );
	}

	inline static Quaternion rotate(const Axes3 &axes, const EulerRotation &euler)
	{
		return Quaternion::rotate( axes.k, euler.z )  *  Quaternion::rotate( axes.j, euler.y )  *  Quaternion::rotate( axes.i, euler.x );
	}

	inline static Quaternion py_rotateEulerInAxes(const Axes3 &axes, const EulerRotation &euler)
	{
		return rotate( axes, euler );
	}

	inline static Quaternion axisToAxis(const Vector3 &src, const Vector3 &dst)
	{
		//find the angle between the vectors
		//and generate a rotation matrix, rotating that amount, where the axis
		//of rotation is the vector that is perpendicular to src and dst
		double cosAngle = src.dot( dst );

		//check if src and dst are the same vector (include margin of error)
		if ( cosAngle  >  1.0 - EPSILON )
		{
			//if so, result is identity rotation
			return Quaternion();
		}
		else if ( cosAngle  <  -1.0 + EPSILON )		//check if they oppose
		{
			//get a vector perpendicular to the src vector
			Vector3 axis = src.perpendicularVector();
			axis.normalise();

			//now get a vector perpendicular to both
			Vector3 axis2 = src.cross( axis );
			axis2.normalise();

			//prefer axis with strongest Y-component
			if ( fabs( axis2.y )  > fabs( axis.y ) )
			{
				axis = axis2;
			}

			//rotate around the chosen axis by 180 degrees
			return Quaternion::rotate( axis, M_PI );
		}
		else
		{
			//calculate rotation axis
			Vector3 axis = src.cross( dst );
			axis.normalise();
			//calculate angle
			double angle = acos( cosAngle );
			//generate the quaternion
			return Quaternion::rotate( axis, angle );
		}
	}



	inline void read(FILE *f)
	{
		fread( &w, sizeof(double), 1, f );
		fread( &x, sizeof(double), 1, f );
		fread( &y, sizeof(double), 1, f );
		fread( &z, sizeof(double), 1, f );
	}

	inline void write(FILE *f) const
	{
		fwrite( &w, sizeof(double), 1, f );
		fwrite( &x, sizeof(double), 1, f );
		fwrite( &y, sizeof(double), 1, f );
		fwrite( &z, sizeof(double), 1, f );
	}
};


#endif
