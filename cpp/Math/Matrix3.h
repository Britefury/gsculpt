//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
/*
  Implemented as follows:

  use the macros iMA, iMB,.... iMP to access the members by letter
  use the macros iMyx where x and y are numbers in the range 0..3 to access the
   members where x and y are the coordinates of the matrix member

  the matrices are implemented so that vectors are represented as ROW vectors,
   rather than columns
*/
#ifndef MATRIX3_H__
#define MATRIX3_H__

#include <boost/python.hpp>



#include <Util/gsassert.h>

#include <Math/Vector3.h>
#include <Math/Point3.h>
#include <Math/Quaternion.h>
#include <Math/Axis.h>
#include <Math/epsilon.h>

//a selection of macros to make acessing the members of the matrix easy
#define iMA d[0]
#define iMB d[1]
#define iMC d[2]
#define iMD d[3]
#define iME d[4]
#define iMF d[5]
#define iMG d[6]
#define iMH d[7]
#define iMI d[8]

#define iM00 d[0]
#define iM01 d[1]
#define iM02 d[2]
#define iM10 d[3]
#define iM11 d[4]
#define iM12 d[5]
#define iM20 d[6]
#define iM21 d[7]
#define iM22 d[8]



//declaration of Vector3 operator*(const Vector3 &v, const Matrix3 &m)
//needed in Matrix3::transformUnit()
class GS_DllExport Matrix3;
inline Vector3 operator*(const Vector3 &v, const Matrix3 &m);


class GS_DllExport Matrix3
{
public:
	double d[9];					//9 (3x3) floats



	//default constructor
	inline Matrix3()
	{
		iM00 = 1.0;	iM01 = 0.0;	iM02 = 0.0;
		iM10 = 0.0;	iM11 = 1.0;	iM12 = 0.0;
		iM20 = 0.0;	iM21 = 0.0;	iM22 = 1.0;
	}

	//constructor taking the data directly
	inline Matrix3(double m00, double m01, double m02,
				double m10, double m11, double m12,
				double m20, double m21, double m22)
	{
		iM00 = m00;	iM01 = m01;	iM02 = m02;
		iM10 = m10;	iM11 = m11;	iM12 = m12;
		iM20 = m20;	iM21 = m21;	iM22 = m22;
	}

	//construct using base vectors (6 methods)
	//base vectors, 3D: 3 methods
	inline Matrix3(const Vector3 &i, const Vector3 &j, const Vector3 &k)
	{
	/*
	i.x		i.y		i.z
	j.x		j.y		j.z
	k.x		k.y		k.z
	*/
		iM00 = i.x;		iM01 = i.y;		iM02 = i.z;
		iM10 = j.x;		iM11 = j.y;		iM12 = j.z;
		iM20 = k.x;		iM21 = k.y;		iM22 = k.z;
	}




	//access methods
	inline double & at(int i, int j)
	{
		return d[ i*3 + j ];
	}

	inline const double & at(int i, int j) const
	{
		return d[ i*3 + j ];
	}


	inline double py__getitem__(boost::python::tuple ij) const
	{
		boost::python::object xiObj = ij[0], xjObj = ij[1];
		boost::python::extract<int> xi( xiObj );
		boost::python::extract<int> xj( xjObj );
		if ( xi.check()  &&  xj.check() )
		{
			int i = xi;
			int j = xj;
			gs_assert( i >= -3  &&  i < 3, "Matrix3::py__getitem__(): @i out of range\n" );
			gs_assert( j >= -3  &&  j < 3, "Matrix3::py__getitem__(): @j out of range\n" );
			if ( i < 0 )
			{
				i += 3;
			}
			if ( j < 0 )
			{
				j += 3;
			}
			return at( i, j );
		}
		else
		{
			return 0.0;
		}
	}

	inline void py__setitem__(boost::python::tuple ij, double value)
	{
		boost::python::object xiObj = ij[0], xjObj = ij[1];
		boost::python::extract<int> xi( xiObj );
		boost::python::extract<int> xj( xjObj );
		if ( xi.check()  &&  xj.check() )
		{
			int i = xi;
			int j = xj;
			gs_assert( i >= -3  &&  i < 3, "Matrix3::py__getitem__(): @i out of range\n" );
			gs_assert( j >= -3  &&  j < 3, "Matrix3::py__getitem__(): @j out of range\n" );
			if ( i < 0 )
			{
				i += 3;
			}
			if ( j < 0 )
			{
				j += 3;
			}
			at( i, j ) = value;
		}
	}



	//mathematical operators
	inline Matrix3 &operator=(const Matrix3 &m)
	{
		iM00 = m.iM00;	iM01 = m.iM01;	iM02 = m.iM02;
		iM10 = m.iM10;	iM11 = m.iM11;	iM12 = m.iM12;
		iM20 = m.iM20;	iM21 = m.iM21;	iM22 = m.iM22;
		return *this;
	}

	inline Matrix3 operator+(const Matrix3 &m) const
	{
		return Matrix3(
			iM00 + m.iM00,	iM01 + m.iM01,	iM02 + m.iM02,
			iM10 + m.iM10,	iM11 + m.iM11,	iM12 + m.iM12,
			iM20 + m.iM20,	iM21 + m.iM21,	iM22 + m.iM22 );
	}

	inline Matrix3 & operator+=(const Matrix3 &m)
	{
		iM00 += m.iM00;		iM01 += m.iM01;		iM02 += m.iM02;
		iM10 += m.iM10;		iM11 += m.iM11;		iM12 += m.iM12;
		iM20 += m.iM20;		iM21 += m.iM21;		iM22 += m.iM22;

		return *this;
	}

	inline Matrix3 operator-(const Matrix3 &m) const
	{
		return Matrix3(
			iM00 - m.iM00,	iM01 - m.iM01,	iM02 - m.iM02,
			iM10 - m.iM10,	iM11 - m.iM11,	iM12 - m.iM12,
			iM20 - m.iM20,	iM21 - m.iM21,	iM22 - m.iM22 );
	}

	inline Matrix3 & operator-=(const Matrix3 &m)
	{
		iM00 -= m.iM00;		iM01 -= m.iM01;		iM02 -= m.iM02;
		iM10 -= m.iM10;		iM11 -= m.iM11;		iM12 -= m.iM12;
		iM20 -= m.iM20;		iM21 -= m.iM21;		iM22 -= m.iM22;

		return *this;
	}

	inline Matrix3 operator*(double s) const
	{
		return Matrix3(
			iM00 * s,	iM01 * s,	iM02 * s,
			iM10 * s,	iM11 * s,	iM12 * s,
			iM20 * s,	iM21 * s,	iM22 * s );
	}

	inline Matrix3 operator*(const Matrix3 &m) const
	{
		return Matrix3(
			iM00 * m.iM00	+	iM01 * m.iM10	+	iM02 * m.iM20,
			iM00 * m.iM01	+	iM01 * m.iM11	+	iM02 * m.iM21,
			iM00 * m.iM02	+	iM01 * m.iM12	+	iM02 * m.iM22,

			iM10 * m.iM00	+	iM11 * m.iM10	+	iM12 * m.iM20,
			iM10 * m.iM01	+	iM11 * m.iM11	+	iM12 * m.iM21,
			iM10 * m.iM02	+	iM11 * m.iM12	+	iM12 * m.iM22,

			iM20 * m.iM00	+	iM21 * m.iM10	+	iM22 * m.iM20,
			iM20 * m.iM01	+	iM21 * m.iM11	+	iM22 * m.iM21,
			iM20 * m.iM02	+	iM21 * m.iM12	+	iM22 * m.iM22 );
	}

	inline Matrix3 operator-() const
	{
		return Matrix3( -iM00,	-iM01,	-iM02,
						 -iM10,	-iM11,	-iM12,
						 -iM20,	-iM21,	-iM22 );
	}



	//transpose
	inline Matrix3 transpose() const
	{
		return Matrix3(
			iM00, iM10, iM20,
			iM01, iM11, iM21,
			iM02, iM12, iM22 );
	}


	//determinant
	inline double determinant() const
	{
		//determinants of 2x2 submatrices
		double d00 = iME*iMI - iMF*iMH;
		double d01 = iMD*iMI - iMF*iMG;
		double d02 = iMD*iMH - iME*iMG;
		return iMA * d00  -  iMB * d01  +  iMC * d02;
	}

	//inverse
	inline bool invert()
	{
		//determinants of 2x2 submatrices
		double d00 = iME*iMI - iMF*iMH;
		double d01 = iMD*iMI - iMF*iMG;
		double d02 = iMD*iMH - iME*iMG;

		double d10 = iMB*iMI - iMC*iMH;
		double d11 = iMA*iMI - iMC*iMG;
		double d12 = iMA*iMH - iMB*iMG;

		double d20 = iMB*iMF - iMC*iME;
		double d21 = iMA*iMF - iMC*iMD;
		double d22 = iMA*iME - iMB*iMD;

		//determinant
		double det = iMA * d00  -  iMB * d01  +  iMC * d02;

		if ( fabs( det )  <=  EPSILON )
		{
			*this = Matrix3();

			return false;
		}
		else
		{
			//reciprocal of the determinant of the 3x3
			double dr = 1.0 / det;

			*this = Matrix3(
				 d00*dr,		-d10*dr,		 d20*dr,
				-d01*dr,		 d11*dr,		-d21*dr,
				 d02*dr,		-d12*dr,		 d22*dr );

			return true;
		}
	}

	inline Matrix3 inverse() const
	{
		//determinants of 2x2 submatrices
		double d00 = iME*iMI - iMF*iMH;
		double d01 = iMD*iMI - iMF*iMG;
		double d02 = iMD*iMH - iME*iMG;

		double d10 = iMB*iMI - iMC*iMH;
		double d11 = iMA*iMI - iMC*iMG;
		double d12 = iMA*iMH - iMB*iMG;

		double d20 = iMB*iMF - iMC*iME;
		double d21 = iMA*iMF - iMC*iMD;
		double d22 = iMA*iME - iMB*iMD;

		//reciprocal of the determinant
		double dr = 1.0  /  ( iMA * d00  -  iMB * d01  +  iMC * d02 );

		return Matrix3(
			 d00*dr,		-d10*dr,		 d20*dr,
			-d01*dr,		 d11*dr,		-d21*dr,
			 d02*dr,		-d12*dr,		 d22*dr );
	}

	//rigid body inverse
	inline Matrix3 rigidBodyInverse() const
	{
		return transpose();
	}





	inline static Matrix3 scale(const Vector3 &s)
	{
		/*
		s.x		0		0
		0		s.y		0
		0		0		s.z
		*/
		return Matrix3(
			s.x,	0.0,	0.0,
			0.0,	s.y,	0.0,
			0.0,	0.0,	s.z );
	}

	inline static Matrix3 py_scaleVector3(const Vector3 &s)
	{
		return scale( s );
	}

	inline static Matrix3 scale(double s)
	{
		/*
		s		0		0
		0		s		0
		0		0		s
		*/
		return Matrix3(
			s, 0.0, 0.0,
			0.0, s, 0.0,
			0.0, 0.0, s );
	}

	inline static Matrix3 scale(double sx, double sy, double sz = 1.0)
	{
		/*
		sx		0		0
		0		sy		0
		0		0		sz
		*/
		return Matrix3(
			sx,	0.0,	0.0,
			0.0,	sy,	0.0,
			0.0,	0.0,	sz );
	}

	inline static Matrix3 py_scale(double s)
	{
		return scale( s );
	}

	inline static Matrix3 py_scale2(double sx, double sy)
	{
		return scale( sx, sy );
	}

	inline static Matrix3 py_scale3(double sx, double sy, double sz)
	{
		return scale( sx, sy, sz );
	}


	//scale in arbitrary axis
	inline static Matrix3 scale(const Vector3 &axis, double s)
	{
		//compute the transformed base vectors
		Vector3 i( 1.0, 0.0, 0.0 );
		Vector3 j( 0.0, 1.0, 0.0 );
		Vector3 k( 0.0, 0.0, 1.0 );

		double sMinus1 = s - 1;

		//computing each vector:
		//
		//v = v  +  axis  *  v dot axis * ( s - 1 )
		//
		i += axis  *  ( axis.x * sMinus1 );		//i dot axis  ==  axis.x
		j += axis  *  ( axis.y * sMinus1 );		//j dot axis  ==  axis.y
		k += axis  *  ( axis.z * sMinus1 );		//k dot axis  ==  axis.z

		//build the matrix
		return Matrix3( i, j, k );
	}

	inline static Matrix3 py_scaleAxis(const Vector3 &axis, double s)
	{
		return scale( axis, s );
	}





	inline static Matrix3 rotateX(double s, double c)
	{
		/*
		let s = sin(a)
			 c = cos(a)

		1		0		0
		0		c		s
		0		-s		c
		*/
		return Matrix3(
			1.0,	0.0,	0.0,
			0.0,	c,	s,
			0.0,	-s,	c );
	}

	inline static Matrix3 rotateX(double a)
	{
		return rotateX( sin( a ), cos( a ) );
	}

	inline static Matrix3 py_rotateXSinCos(double s, double c)
	{
		return rotateX( s, c );
	}

	inline static Matrix3 py_rotateX(double a)
	{
		return rotateX( a );
	}


	inline static Matrix3 rotateY(double s, double c)
	{
		/*
		let s = sin(a)
			 c = cos(a)

		c		0		-s
		0		1		0
		s		0		c
		*/
		return Matrix3(
			c,	0.0,	-s,
			0.0,	1.0,	0.0,
			s,	0.0,	c );
	}

	inline static Matrix3 rotateY(double a)
	{
		return rotateY( sin( a ), cos( a ) );
	}

	inline static Matrix3 py_rotateYSinCos(double s, double c)
	{
		return rotateY( s, c );
	}

	inline static Matrix3 py_rotateY(double a)
	{
		return rotateY( a );
	}


	inline static Matrix3 rotateZ(double s, double c)
	{
		/*
		let s = sin(a)
			 c = cos(a)

		c		s		0
		-s		c		0
		0		0		1
		*/
		return Matrix3(
			c,	s,	0.0,
			-s,	c,	0.0,
			0.0,	0.0,	1.0 );
	}

	inline static Matrix3 rotateZ(double a)
	{
		return rotateZ( sin( a ), cos( a ) );
	}

	inline static Matrix3 py_rotateZSinCos(double s, double c)
	{
		return rotateZ( s, c );
	}

	inline static Matrix3 py_rotateZ(double a)
	{
		return rotateZ( a );
	}



	inline static Matrix3 rotate(Axis axis, double a)
	{
		switch( axis )
		{
		case AXIS_X:
			return rotateX( a );
			break;
		case AXIS_NEGATIVE_X:
			return rotateX( -a );
			break;
		case AXIS_Y:
			return rotateY( a );
			break;
		case AXIS_NEGATIVE_Y:
			return rotateY( -a );
			break;
		case AXIS_Z:
			return rotateZ( a );
			break;
		case AXIS_NEGATIVE_Z:
			return rotateZ( -a );
			break;
		default:
			return Matrix3();
			break;
		}
	}

	inline static Matrix3 rotate(const Vector3 &axis, double a)
	{
		double w, x, y, z;
		double xx, yy, zz, xy, xz, yz, wx, wy, wz;
		double s;

		a *= 0.5;
		s = sin( a );
		w = cos( a );
		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;

		xx = x * x * 2.0;		yy = y * y * 2.0;		zz = z * z * 2.0;
		xy = x * y * 2.0;		xz = x * z * 2.0;		yz = y * z * 2.0;
		wx = w * x * 2.0;		wy = w * y * 2.0;		wz = w * z * 2.0;

		/*
		the quaternion q:
								w=cos(a/2)
								x=axis.x*sin(a/2)
								y=axis.y*sin(a/2)
								z=axis.z*sin(a/2) represents the rotation

		matrix:
		1 - 2yy - 2zz		2xy + 2wz		2xz - 2wy
		2xy - 2wz		1 - 2xx - 2zz		2yz + 2wx
		2xz + 2wy		2yz - 2wx		1 - 2xx - 2yy
		*/
		return Matrix3(
			1.0 - (yy + zz),	xy + wz,			xz - wy,
			xy - wz,			1.0 - (xx + zz),	yz + wx,
			xz + wy,			yz - wx,			1.0 - (xx + yy) );
	}

	inline static Matrix3 rotate(const Quaternion &q)
	{
		double xx, yy, zz, xy, xz, yz, wx, wy, wz;

		xx = q.x * q.x * 2.0;	yy = q.y * q.y * 2.0;	zz = q.z * q.z * 2.0;
		xy = q.x * q.y * 2.0;	xz = q.x * q.z * 2.0;	yz = q.y * q.z * 2.0;
		wx = q.w * q.x * 2.0;		wy = q.w * q.y * 2.0;		wz = q.w * q.z * 2.0;


		// matrix:
		// 1 - 2yy - 2zz	2xy + 2wz		2xz - 2wy
		// 2xy - 2wz		1 - 2xx - 2zz		2yz + 2wx
		// 2xz + 2wy		2yz - 2wx		1 - 2xx - 2yy

		return Matrix3(
			1.0 - (yy + zz),	xy + wz,			xz - wy,
			xy - wz,			1.0 - (xx + zz),	yz + wx,
			xz + wy,			yz - wx,			1.0 - (xx + yy) );
	}

	inline static Matrix3 py_rotateAxis(Axis axis, double a)
	{
		return rotate( axis, a );
	}

	inline static Matrix3 py_rotateAxisVector(const Vector3 &axis, double a)
	{
		return rotate( axis, a );
	}

	inline static Matrix3 py_rotateQuaternion(const Quaternion &q)
	{
		return rotate( q );
	}





	inline static Matrix3 shearX(double yShear, double zShear)
	{
		/*
		1			sy/sx		sz/sx
		0			1			0
		0			0			1
		*/
		return Matrix3(
				1.0,			yShear,		zShear,
				0.0,			1.0,			0.0,
				0.0,			0.0,			1.0 );
	}

	inline static Matrix3 shearY(double xShear, double zShear)
	{
		/*
		1			0			0
		sx/sy		1			sz/sy
		0			0			1
		*/
		return Matrix3(
				1.0,			0.0,			0.0,
				xShear,		1.0,			zShear,
				0.0,			0.0,			1.0 );
	}

	inline static Matrix3 shearZ(double xShear, double yShear)
	{
		/*
		1			0			0
		0			1			0
		sx/sz		sy/sz		1
		*/
		return Matrix3(
				1.0,			0.0,			0.0,
				0.0,			1.0,			0.0,
				xShear,		yShear,		1.0 );
	}

	inline static Matrix3 shear(const Vector3 &shearAxis, const Vector3 &shearVector)
	{
		double i = shearAxis.x;			// [1,0,0] (dot) shearAxis
		double j = shearAxis.y;			// [0,1,0] (dot) shearAxis
		double k = shearAxis.z;			// [0,0,1] (dot) shearAxis

		//compute the base vectors
		Vector3 baseI = Vector3( 1.0, 0.0, 0.0 )  +  shearVector * i;
		Vector3 baseJ = Vector3( 0.0, 1.0, 0.0 )  +  shearVector * j;
		Vector3 baseK = Vector3( 0.0, 0.0, 1.0 )  +  shearVector * k;

		return Matrix3( baseI, baseJ, baseK );
	}





	inline static Matrix3 reflect(const Vector3 &n)
	{
		double nx = -2.0 * n.x,   ny = -2.0 * n.y,   nz = -2.0 * n.z;

			/*
			1 - 2xx		-2xy			-2xz
			-2yx			1 - 2yy		-2yz
			-2zx			-2zy			1 - 2zz
			*/
		return Matrix3(
			1.0 + nx * n.x,	nx * n.y, 		nx * n.z,
			ny * n.x,			1.0 + ny * n.y,	ny * n.z,
			nz * n.x,			nz * n.y,			1.0 + nz * n.z );
	}



	inline static Matrix3 cross(const Vector3 &v)
	{
		/*
		0		-v.z		v.y
		v.z		0		-v.x
		-v.y		v.x		0
		*/
		return Matrix3(
			0.0,		-v.z,		v.y,
			v.z,		0.0,		-v.x,
			-v.y,		v.x,		0.0 );
	}





	inline static Matrix3 spaceToSpace(const Vector3 &i, const Vector3 &j,
										const Vector3 &k, const Vector3 &u,
										const Vector3 &v, const Vector3 &w)
	{
		//inverse of matrix ijk multiplied by matrix uvw
		return Matrix3( i, j, k ).inverse()  *  Matrix3( u, v, w );
	}




	inline static Matrix3 axisToAxis(const Vector3 &src, const Vector3 &dst)
	{
		//find the angle between the vectors
		//and generate a rotation matrix, rotating that amount, where the axis
		//of rotation is the vector that is perpendicular to src and dst
		double cosAngle = src.dot( dst );

		//check if src and dst are the same vector (include margin of error)
		if ( cosAngle  >  1.0 - EPSILON )
		{
			//if so, result is identity matrix
			return Matrix3();
		}
		else if ( cosAngle  <  -1.0 + EPSILON )		//check if they oppose
		{
			//get a vector perpendicular to the src vector
			Vector3 axis = src.perpendicularVector();
			axis.normalise();

			//rotate around this by 180 degrees
			return rotate( axis, M_PI * 2.0 );
		}
		else
		{
			//calculate rotation axis
			Vector3 axis = src.cross( dst );
			axis.normalise();
			//calculate angle
			double angle = acos( cosAngle );
			//generate the matrix
			return rotate( axis, angle );
		}
	}



	inline static Matrix3 lookat(const Vector3 &lookDirection, const Vector3 &up)
	{
		Vector3 right = lookDirection.cross( up );

		if ( right.sqrLength() < SQR_EPSILON )
		{
			right = Vector3( 1.0, 0.0, 0.0 );
		}

		Vector3 realUp = right.cross( lookDirection );

		right.normalise();
		realUp.normalise();

		return Matrix3( right, realUp, -lookDirection.getNormalised() );
	}







	//transform a unit vector, ensuring thats its length squared is within
	//the range [minLengthSquared,maxLengthSquared]
	inline Vector3 transformUnit(const Vector3 &v, double minLengthSquared, double maxLengthSquared)
	{
		Vector3 result = v * (*this);
		double l = result.sqrLength();
		if ( ( l < minLengthSquared )  ||  ( l > maxLengthSquared ) )
		{
			//length not within [minLengthSquared,maxLengthSquared],
			//normalise result
			result.normalise();
		}

		return result;
	}
};


//multiply Point3 by a matrix; transform the point
inline Point3 operator*(const Point3 &p, const Matrix3 &m)
{
	return Point3(
		p.x * m.iM00  +  p.y * m.iM10  +  p.z * m.iM20,
		p.x * m.iM01  +  p.y * m.iM11  +  p.z * m.iM21,
		p.x * m.iM02  +  p.y * m.iM12  +  p.z * m.iM22 );
}

//multiply Vector3 by a matrix; transform the vector
inline Vector3 operator*(const Vector3 &v, const Matrix3 &m)
{
	return Vector3(
		v.x * m.iM00  +  v.y * m.iM10  +  v.z * m.iM20,
		v.x * m.iM01  +  v.y * m.iM11  +  v.z * m.iM21,
		v.x * m.iM02  +  v.y * m.iM12  +  v.z * m.iM22 );
}



inline void operator*=(Point3 &p, const Matrix3 &m)
{
	double x =	p.x * m.iM00  +  p.y * m.iM10  +  p.z * m.iM20;
	double y =	p.x * m.iM01  +  p.y * m.iM11  +  p.z * m.iM21;
	p.z =		p.x * m.iM02  +  p.y * m.iM12  +  p.z * m.iM22;
	p.x = x;
	p.y = y;
}

inline void operator*=(Vector3 &v, const Matrix3 &m)
{
	double x =	v.x * m.iM00  +  v.y * m.iM10  +  v.z * m.iM20;
	double y =	v.x * m.iM01  +  v.y * m.iM11  +  v.z * m.iM21;
	v.z =		v.x * m.iM02  +  v.y * m.iM12  +  v.z * m.iM22;
	v.x = x;
	v.y = y;
}



#undef iMA
#undef iMB
#undef iMC
#undef iMD
#undef iME
#undef iMF
#undef iMG
#undef iMH
#undef iMI

#undef iM00
#undef iM01
#undef iM02
#undef iM10
#undef iM11
#undef iM12
#undef iM20
#undef iM21
#undef iM22


#endif
