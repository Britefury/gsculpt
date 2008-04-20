//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
/*	by:					Geoffrey French
	last revision:		14/Nov/2002
	title:				4x4 transformation matrix class with mathematical operations.

  Implemented as follows:

  use the macros iMA, iMB,.... iMP to access the members by letter
  use the macros iMyx where x and y are numbers in the range 0..3 to access the
   members where x and y are the coordinates of the matrix member

  the matrices are implemented so that vectors are represented as ROW vectors,
   rather than columns
*/
#ifndef MATRIX4_H__
#define MATRIX4_H__

#include <boost/python.hpp>



#include <stdio.h>

#include <Util/gsassert.h>
#include <Util/SETree.h>

#include <Math/radians.h>

#include <Math/Vector2.h>
#include <Math/Vector2f.h>
#include <Math/Vector3.h>
#include <Math/Vector3f.h>
#include <Math/Point2f.h>
#include <Math/Point2.h>
#include <Math/Point3.h>
#include <Math/Point3f.h>
#include <Math/Point4.h>
#include <Math/Plane.h>
#include <Math/EulerRotation.h>
#include <Math/Quaternion.h>
#include <Math/Axes3.h>
#include <Math/Segment3.h>
#include <Math/Segment2.h>
#include <Math/Triangle2.h>
#include <Math/Triangle3.h>
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
#define iMJ d[9]
#define iMK d[10]
#define iML d[11]
#define iMM d[12]
#define iMN d[13]
#define iMO d[14]
#define iMP d[15]

#define iM00 d[0]
#define iM01 d[1]
#define iM02 d[2]
#define iM03 d[3]
#define iM10 d[4]
#define iM11 d[5]
#define iM12 d[6]
#define iM13 d[7]
#define iM20 d[8]
#define iM21 d[9]
#define iM22 d[10]
#define iM23 d[11]
#define iM30 d[12]
#define iM31 d[13]
#define iM32 d[14]
#define iM33 d[15]



//declaration of Vector3 operator*(const Vector3 &v, const Matrix4 &m)
//needed in Matrix4::transformUnit()
class GS_DllExport Matrix4;
inline Vector3 operator*(const Vector3 &v, const Matrix4 &m);


class GS_DllExport Matrix4
{
public:
	double d[16];					//16 (4x4) floats



	//default constructor
	inline Matrix4()
	{
		iM00 = 1.0;	iM01 = 0.0;	iM02 = 0.0;	iM03 = 0.0;
		iM10 = 0.0;	iM11 = 1.0;	iM12 = 0.0;	iM13 = 0.0;
		iM20 = 0.0;	iM21 = 0.0;	iM22 = 1.0;	iM23 = 0.0;
		iM30 = 0.0;	iM31 = 0.0;	iM32 = 0.0;	iM33 = 1.0;
	}

	//constructor taking the data directly
	inline Matrix4(double m00, double m01, double m02, double m03,
				double m10, double m11, double m12, double m13,
				double m20, double m21, double m22, double m23,
				double m30, double m31, double m32, double m33)
	{
		iM00 = m00;	iM01 = m01;	iM02 = m02;	iM03 = m03;
		iM10 = m10;	iM11 = m11;	iM12 = m12;	iM13 = m13;
		iM20 = m20;	iM21 = m21;	iM22 = m22;	iM23 = m23;
		iM30 = m30;	iM31 = m31;	iM32 = m32;	iM33 = m33;
	}

	//construct using base vectors (6 methods)
	//base vectors, 3D: 3 methods
	inline Matrix4(const Vector3 &i, const Vector3 &j, const Vector3 &k)
	{
	/*
	i.x		i.y		i.z		0
	j.x		j.y		j.z		0
	k.x		k.y		k.z		0
	0.0		0.0		0.0		1
	*/
		iM00 = i.x;		iM01 = i.y;		iM02 = i.z;		iM03 = 0.0;
		iM10 = j.x;		iM11 = j.y;		iM12 = j.z;		iM13 = 0.0;
		iM20 = k.x;		iM21 = k.y;		iM22 = k.z;		iM23 = 0.0;
		iM30 = 0.0;		iM31 = 0.0;		iM32 = 0.0;		iM33 = 1.0;
	}

	inline Matrix4(const Vector3 &i, const Vector3 &j, const Vector3 &k, const Vector3 &xlat)
	{
	/*
	i.x		i.y		i.z		0
	j.x		j.y		j.z		0
	k.x		k.y		k.z		0
	xlat.x	xlat.y	xlat.z	1
	*/
		iM00 = i.x;		iM01 = i.y;		iM02 = i.z;		iM03 = 0.0;
		iM10 = j.x;		iM11 = j.y;		iM12 = j.z;		iM13 = 0.0;
		iM20 = k.x;		iM21 = k.y;		iM22 = k.z;		iM23 = 0.0;
		iM30 = xlat.x;		iM31 = xlat.y;		iM32 = xlat.z;		iM33 = 1.0;
	}

	inline Matrix4(const Point3 &o, const Vector3 &i, const Vector3 &j, const Vector3 &k)
	{
	/*
	i.x		i.y		i.z		0
	j.x		j.y		j.z		0
	k.x		k.y		k.z		0
	o.x		o.y		o.z		1
	*/
		iM00 = i.x;		iM01 = i.y;		iM02 = i.z;		iM03 = 0.0;
		iM10 = j.x;		iM11 = j.y;		iM12 = j.z;		iM13 = 0.0;
		iM20 = k.x;		iM21 = k.y;		iM22 = k.z;		iM23 = 0.0;
		iM30 = o.x;		iM31 = o.y;		iM32 = o.z;		iM33 = 1.0;
	}

	inline Matrix4(const Point3 &o, const Point3 &oi, const Point3 &oj, const Point3 &ok)
	{
		Vector3 i = oi - o;
		Vector3 j = oj - o;
		Vector3 k = ok - o;
	/*
	i.x		i.y		i.z		0
	j.x		j.y		j.z		0
	k.x		k.y		k.z		0
	o.x		o.y		o.z		1
	*/
		iM00 = i.x;		iM01 = i.y;		iM02 = i.z;		iM03 = 0.0;
		iM10 = j.x;		iM11 = j.y;		iM12 = j.z;		iM13 = 0.0;
		iM20 = k.x;		iM21 = k.y;		iM22 = k.z;		iM23 = 0.0;
		iM30 = o.x;		iM31 = o.y;		iM32 = o.z;		iM33 = 1.0;
	}

	//base vectors, 2D: 3 methods
	inline Matrix4(const Vector2 &i, const Vector2 &j)
	{
	/*
	i.x		i.y		0			0
	j.x		j.y		0			0
	0		0		1			0
	0		0		0			1
	*/
		iM00 = i.x;		iM01 = i.y;		iM02 = 0.0;		iM03 = 0.0;
		iM10 = j.x;		iM11 = j.y;		iM12 = 0.0;		iM13 = 0.0;
		iM20 = 0.0;		iM21 = 0.0;		iM22 = 1.0;		iM23 = 0.0;
		iM30 = 0.0;		iM31 = 0.0;		iM32 = 0.0;		iM33 = 1.0;
	}

	//base vectors, 2D: 3 methods
	inline Matrix4(const Vector2 &i, const Vector2 &j, const Vector2 &xlat)
	{
	/*
	i.x		i.y		0			0
	j.x		j.y		0			0
	0			0			1			0
	xlat.x	xlat.y	0			1
	*/
		iM00 = i.x;		iM01 = i.y;		iM02 = 0.0;		iM03 = 0.0;
		iM10 = j.x;		iM11 = j.y;		iM12 = 0.0;		iM13 = 0.0;
		iM20 = 0.0;		iM21 = 0.0;		iM22 = 1.0;		iM23 = 0.0;
		iM30 = xlat.x;		iM31 = xlat.y;		iM32 = 0.0;		iM33 = 1.0;
	}

	inline Matrix4(const Point2 &o, const Vector2 &i, const Vector2 &j)
	{
	/*
	i.x		i.y		0			0
	j.x		j.y		0			0
	0			0			1			0
	o.x		o.y		0			1
	*/
		iM00 = i.x;		iM01 = i.y;		iM02 = 0.0;		iM03 = 0.0;
		iM10 = j.x;		iM11 = j.y;		iM12 = 0.0;		iM13 = 0.0;
		iM20 = 0.0;		iM21 = 0.0;		iM22 = 1.0;		iM23 = 0.0;
		iM30 = o.x;		iM31 = o.y;		iM32 = 0.0;		iM33 = 1.0;
	}

	inline Matrix4(const Point2 &o, const Point2 &oi, const Point2 &oj)
	{
		Vector2 i = oi - o;
		Vector2 j = oj - o;
	/*
	i.x		i.y		0			0
	j.x		j.y		0			0
	0			0			1			0
	o.x		o.y		0			1
	*/
		iM00 = i.x;		iM01 = i.y;		iM02 = 0.0;		iM03 = 0.0;
		iM10 = j.x;		iM11 = j.y;		iM12 = 0.0;		iM13 = 0.0;
		iM20 = 0.0;		iM21 = 0.0;		iM22 = 1.0;		iM23 = 0.0;
		iM30 = o.x;		iM31 = o.y;		iM32 = 0.0;		iM33 = 1.0;
	}




	//read and write methods
	inline void read(FILE *f)
	{
		fread( d, sizeof(double), 16, f );
	}

	inline void write(FILE *f) const
	{
		fwrite( d, sizeof(double), 16, f );
	}

	//read and write methods
	inline void read(SENode &node)
	{
		node[0]  >>  d[0];
		node[1]  >>  d[1];
		node[2]  >>  d[2];
		node[3]  >>  d[3];
		node[4]  >>  d[4];
		node[5]  >>  d[5];
		node[6]  >>  d[6];
		node[7]  >>  d[7];
		node[8]  >>  d[8];
		node[9]  >>  d[9];
		node[10]  >>  d[10];
		node[11]  >>  d[11];
		node[12]  >>  d[12];
		node[13]  >>  d[13];
		node[14]  >>  d[14];
		node[15]  >>  d[15];
	}

	inline void write(SENode &node) const
	{
		node.addItem()  <<  d[0];
		node.addItem()  <<  d[1];
		node.addItem()  <<  d[2];
		node.addItem()  <<  d[3];
		node.addItem()  <<  d[4];
		node.addItem()  <<  d[5];
		node.addItem()  <<  d[6];
		node.addItem()  <<  d[7];
		node.addItem()  <<  d[8];
		node.addItem()  <<  d[9];
		node.addItem()  <<  d[10];
		node.addItem()  <<  d[11];
		node.addItem()  <<  d[12];
		node.addItem()  <<  d[13];
		node.addItem()  <<  d[14];
		node.addItem()  <<  d[15];
	}




	//access methods
	inline double & at(int i, int j)
	{
		return d[ i*4 + j ];
	}

	inline const double & at(int i, int j) const
	{
		return d[ i*4 + j ];
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
			gs_assert( i >= -4  &&  i < 4, "Matrix4::py__getitem__(): @i out of range\n" );
			gs_assert( j >= -4  &&  j < 4, "Matrix4::py__getitem__(): @j out of range\n" );
			if ( i < 0 )
			{
				i += 4;
			}
			if ( j < 0 )
			{
				j += 4;
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
			gs_assert( i >= -4  &&  i < 4, "Matrix4::py__getitem__(): @i out of range\n" );
			gs_assert( j >= -4  &&  j < 4, "Matrix4::py__getitem__(): @j out of range\n" );
			if ( i < 0 )
			{
				i += 4;
			}
			if ( j < 0 )
			{
				j += 4;
			}
			at( i, j ) = value;
		}
	}



	//mathematical operators
	inline Matrix4 &operator=(const Matrix4 &m)
	{
		iM00 = m.iM00;	iM01 = m.iM01;	iM02 = m.iM02;	iM03 = m.iM03;
		iM10 = m.iM10;	iM11 = m.iM11;	iM12 = m.iM12;	iM13 = m.iM13;
		iM20 = m.iM20;	iM21 = m.iM21;	iM22 = m.iM22;	iM23 = m.iM23;
		iM30 = m.iM30;	iM31 = m.iM31;	iM32 = m.iM32;	iM33 = m.iM33;
		return *this;
	}

	inline Matrix4 operator+(const Matrix4 &m) const
	{
		return Matrix4(
			iM00 + m.iM00,	iM01 + m.iM01,	iM02 + m.iM02,	iM03 + m.iM03,
			iM10 + m.iM10,	iM11 + m.iM11,	iM12 + m.iM12,	iM13 + m.iM13,
			iM20 + m.iM20,	iM21 + m.iM21,	iM22 + m.iM22,	iM23 + m.iM23,
			iM30 + m.iM30,	iM31 + m.iM31,	iM32 + m.iM32,	iM33 + m.iM33 );
	}

	inline Matrix4 operator-(const Matrix4 &m) const
	{
		return Matrix4(
			iM00 - m.iM00,	iM01 - m.iM01,	iM02 - m.iM02,	iM03 - m.iM03,
			iM10 - m.iM10,	iM11 - m.iM11,	iM12 - m.iM12,	iM13 - m.iM13,
			iM20 - m.iM20,	iM21 - m.iM21,	iM22 - m.iM22,	iM23 - m.iM23,
			iM30 - m.iM30,	iM31 - m.iM31,	iM32 - m.iM32,	iM33 - m.iM33 );
	}

	inline Matrix4 operator*(double s) const
	{
		return Matrix4(
			iM00 * s,	iM01 * s,	iM02 * s,	iM03 * s,
			iM10 * s,	iM11 * s,	iM12 * s,	iM13 * s,
			iM20 * s,	iM21 * s,	iM22 * s,	iM23 * s,
			iM30 * s,	iM31 * s,	iM32 * s,	iM33 * s );
	}

	inline Matrix4 operator*(const Matrix4 &m) const
	{
		return Matrix4(
			iM00 * m.iM00	+	iM01 * m.iM10	+	iM02 * m.iM20	+	iM03 * m.iM30,
			iM00 * m.iM01	+	iM01 * m.iM11	+	iM02 * m.iM21	+	iM03 * m.iM31,
			iM00 * m.iM02	+	iM01 * m.iM12	+	iM02 * m.iM22	+	iM03 * m.iM32,
			iM00 * m.iM03	+	iM01 * m.iM13	+	iM02 * m.iM23	+	iM03 * m.iM33,

			iM10 * m.iM00	+	iM11 * m.iM10	+	iM12 * m.iM20	+	iM13 * m.iM30,
			iM10 * m.iM01	+	iM11 * m.iM11	+	iM12 * m.iM21	+	iM13 * m.iM31,
			iM10 * m.iM02	+	iM11 * m.iM12	+	iM12 * m.iM22	+	iM13 * m.iM32,
			iM10 * m.iM03	+	iM11 * m.iM13	+	iM12 * m.iM23	+	iM13 * m.iM33,

			iM20 * m.iM00	+	iM21 * m.iM10	+	iM22 * m.iM20	+	iM23 * m.iM30,
			iM20 * m.iM01	+	iM21 * m.iM11	+	iM22 * m.iM21	+	iM23 * m.iM31,
			iM20 * m.iM02	+	iM21 * m.iM12	+	iM22 * m.iM22	+	iM23 * m.iM32,
			iM20 * m.iM03	+	iM21 * m.iM13	+	iM22 * m.iM23	+	iM23 * m.iM33,

			iM30 * m.iM00	+	iM31 * m.iM10	+	iM32 * m.iM20	+	iM33 * m.iM30,
			iM30 * m.iM01	+	iM31 * m.iM11	+	iM32 * m.iM21	+	iM33 * m.iM31,
			iM30 * m.iM02	+	iM31 * m.iM12	+	iM32 * m.iM22	+	iM33 * m.iM32,
			iM30 * m.iM03	+	iM31 * m.iM13	+	iM32 * m.iM23	+	iM33 * m.iM33 );
	}

	inline Matrix4 operator-() const
	{
		return Matrix4( -iM00,	-iM01,	-iM02,	-iM03,
							 -iM10,	-iM11,	-iM12,	-iM13,
							 -iM20,	-iM21,	-iM22,	-iM23,
							 -iM30,	-iM31,	-iM32,	-iM33 );
	}


	//determinant
	inline double determinant() const
	{
		//determinants of 2x2 submatrices
		double kplo = iMK*iMP-iML*iMO,	jpln = iMJ*iMP-iML*iMN,	jokn = iMJ*iMO-iMK*iMN;
		double iplm = iMI*iMP-iML*iMM,	iokm = iMI*iMO-iMK*iMM,	injm = iMI*iMN-iMJ*iMM;
		//determinants of 3x3 submatrices
		double d00 = iMA * (iMF*kplo - iMG*jpln + iMH*jokn);
		double d01 = iMB * (iME*kplo - iMG*iplm + iMH*iokm);
		double d02 = iMC * (iME*jpln - iMF*iplm + iMH*injm);
		double d03 = iMD * (iME*jokn - iMF*iokm + iMG*injm);
		//determinant of the 4x4
		return (iMA*d00 - iMB*d01 + iMC*d02 - iMD*d03);
	}

	//inverse
	inline Matrix4 inverse() const
	{
		//determinants of 2x2 submatrices
		double kplo = iMK*iMP-iML*iMO, jpln = iMJ*iMP-iML*iMN,	jokn = iMJ*iMO-iMK*iMN;
		double iplm = iMI*iMP-iML*iMM, iokm = iMI*iMO-iMK*iMM,	injm = iMI*iMN-iMJ*iMM;
		double gpho = iMG*iMP-iMH*iMO, ifhn = iMF*iMP-iMH*iMN,	fogn = iMF*iMO-iMG*iMN;
		double ephm = iME*iMP-iMH*iMM, eogm = iME*iMO-iMG*iMM,	enfm = iME*iMN-iMF*iMM;
		double glhk = iMG*iML-iMH*iMK, flhj = iMF*iML-iMH*iMJ,	fkgj = iMF*iMK-iMG*iMJ;
		double elhi = iME*iML-iMH*iMI, ekgi = iME*iMK-iMG*iMI,	ejfi = iME*iMJ-iMF*iMI;
		//determinants of 3x3 submatrices
		double d00 = (iMF*kplo - iMG*jpln + iMH*jokn);
		double d01 = (iME*kplo - iMG*iplm + iMH*iokm);
		double d02 = (iME*jpln - iMF*iplm + iMH*injm);
		double d03 = (iME*jokn - iMF*iokm + iMG*injm);

		double d10 = (iMB*kplo - iMC*jpln + iMD*jokn);
		double d11 = (iMA*kplo - iMC*iplm + iMD*iokm);
		double d12 = (iMA*jpln - iMB*iplm + iMD*injm);
		double d13 = (iMA*jokn - iMB*iokm + iMC*injm);

		double d20 = (iMB*gpho - iMC*ifhn + iMD*fogn);
		double d21 = (iMA*gpho - iMC*ephm + iMD*eogm);
		double d22 = (iMA*ifhn - iMB*ephm + iMD*enfm);
		double d23 = (iMA*fogn - iMB*eogm + iMC*enfm);

		double d30 = (iMB*glhk - iMC*flhj + iMD*fkgj);
		double d31 = (iMA*glhk - iMC*elhi + iMD*ekgi);
		double d32 = (iMA*flhj - iMB*elhi + iMD*ejfi);
		double d33 = (iMA*fkgj - iMB*ekgi + iMC*ejfi);
		//reciprocal of the determinant of the 4x4
		double dr = 1.0 / (iMA*d00 - iMB*d01 + iMC*d02 - iMD*d03);

		return Matrix4( d00*dr,		-d10*dr,		 d20*dr,	-d30*dr,
							 -d01*dr,	 d11*dr,		-d21*dr,	 d31*dr,
							  d02*dr,	-d12*dr,		 d22*dr,	-d32*dr,
							 -d03*dr,	 d13*dr,		-d23*dr,	 d33*dr);
	}

	//rigid body inverse
	inline Matrix4 rigidBodyInverse() const
	{
		return Matrix4(  iM00,   iM10,   iM20,   0.0,
							  iM01,   iM11,   iM21,   0.0,
							  iM02,   iM12,   iM22,   0.0,
							  -iM30 * iM00  +  -iM31 * iM01  +  -iM32 * iM02,
							  -iM30 * iM10  +  -iM31 * iM11  +  -iM32 * iM12,
							  -iM30 * iM20  +  -iM31 * iM21  +  -iM32 * iM22,
							  1.0 );
	}


	//interpolate
	inline static Matrix4 weightedAverage(const Matrix4 &a, double wa,
													  const Matrix4 &b, double wb)
	{
		return Matrix4(
			a.iM00 * wa  +  b.iM00 * wb,		a.iM01 * wa  +  b.iM01 * wb,
			a.iM02 * wa  +  b.iM02 * wb,		a.iM03 * wa  +  b.iM03 * wb,
			a.iM10 * wa  +  b.iM10 * wb,		a.iM11 * wa  +  b.iM11 * wb,
			a.iM12 * wa  +  b.iM12 * wb,		a.iM13 * wa  +  b.iM13 * wb,
			a.iM20 * wa  +  b.iM20 * wb,		a.iM21 * wa  +  b.iM21 * wb,
			a.iM22 * wa  +  b.iM22 * wb,		a.iM23 * wa  +  b.iM23 * wb,
			a.iM30 * wa  +  b.iM30 * wb,		a.iM31 * wa  +  b.iM31 * wb,
			a.iM32 * wa  +  b.iM32 * wb,		a.iM33 * wa  +  b.iM33 * wb );
	}

	//interpolate
	inline static Matrix4 lerp(const Matrix4 &a, const Matrix4 &b, double t)
	{
		return a  +  ( b - a ) * t;
	}






	inline static Matrix4 translate(const Vector3 &t)
	{
		/*
		1		0		0		0
		0		1		0		0
		0		0		1		0
		t.x	t.y	t.z	1
		*/
		return Matrix4(
			1.0,	0.0,	0.0,	0.0,
			0.0,	1.0,	0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			t.x,	t.y,	t.z,	1.0);
	}

	inline static Matrix4 py_translateVector3(const Vector3 &t)
	{
		return translate( t );
	}

	inline static Matrix4 translate(const Vector2 &t)
	{
		/*
		1		0		0		0
		0		1		0		0
		0		0		1		0
		t.x	t.y	0		1
		*/
		return Matrix4(
			1.0,	0.0,	0.0,	0.0,
			0.0,	1.0,	0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			t.x,	t.y,	0.0,	1.0);
	}

	inline static Matrix4 py_translateVector2(const Vector2 &t)
	{
		return translate( t );
	}

	inline static Matrix4 translate(double tx, double ty, double tz = 0.0)
	{
		/*
		1		0		0		0
		0		1		0		0
		0		0		1		0
		t.x	t.y	t.z	1
		*/
		return Matrix4(
			1.0,	0.0,	0.0,	0.0,
			0.0,	1.0,	0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			tx,	ty,	tz,	1.0);
	}

	inline static Matrix4 py_translate3(double x, double y, double z)
	{
		return translate( x, y, z );
	}

	inline static Matrix4 py_translate2(double x, double y)
	{
		return translate( x, y );
	}





	inline static Matrix4 scale(const Vector3 &s)
	{
		/*
		s.x	0		0		0
		0		s.y	0		0
		0		0		s.z	0
		0		0		0		1
		*/
		return Matrix4(
			s.x,	0.0,	0.0,	0.0,
			0.0,	s.y,	0.0,	0.0,
			0.0,	0.0,	s.z,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 py_scaleVector3(const Vector3 &s)
	{
		return scale( s );
	}

	inline static Matrix4 scale(const Vector2 &s)
	{
		/*
		s.x	0		0		0
		0		s.y	0		0
		0		0		1.0	0
		0		0		0		1
		*/
		return Matrix4(
			s.x,	0.0,	0.0,	0.0,
			0.0,	s.y,	0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 py_scaleVector2(const Vector2 &s)
	{
		return scale( s );
	}

	inline static Matrix4 scale(double sx, double sy, double sz = 1.0)
	{
		/*
		sx		0		0		0
		0		sy		0		0
		0		0		sz		0
		0		0		0		1
		*/
		return Matrix4(
			sx,	0.0,	0.0,	0.0,
			0.0,	sy,	0.0,	0.0,
			0.0,	0.0,	sz,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 scale(double s)
	{
		/*
		s		0		0		0
		0		s		0		0
		0		0		s		0
		0		0		0		1
		*/
		return Matrix4(
			s,		0.0,	0.0,	0.0,
			0.0,	s,		0.0,	0.0,
			0.0,	0.0,	s,		0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 py_scale(double s)
	{
		return scale( s );
	}

	inline static Matrix4 py_scale2(double x, double y)
	{
		return scale( x, y );
	}

	inline static Matrix4 py_scale3(double x, double y, double z)
	{
		return scale( x, y, z );
	}



	//scale in arbitrary axis
	inline static Matrix4 scale(const Vector3 &axis, double s)
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
		return Matrix4( i, j, k );
	}

	inline static Matrix4 py_scaleAxis3(const Vector3 &axis, double s)
	{
		return scale( axis, s );
	}

	//scale in arbitrary axis (2D)
	inline static Matrix4 scale(const Vector2 &axis, double s)
	{
		//compute the transformed base vectors
		Vector2 i( 1.0, 0.0 );
		Vector2 j( 0.0, 1.0 );

		double sMinus1 = s - 1;

		//computing each vector:
		//
		//v = v  +  axis  *  v dot axis * ( s - 1 )
		//
		i += axis  *  ( axis.x * sMinus1 );		//i dot axis  ==  axis.x
		j += axis  *  ( axis.y * sMinus1 );		//j dot axis  ==  axis.y

		//build the matrix
		return Matrix4( i, j );
	}

	inline static Matrix4 py_scaleAxis2(const Vector2 &axis, double s)
	{
		return scale( axis, s );
	}





	inline static Matrix4 rotateX(double a)
	{
		/*
		let s = sin(a)
			 c = cos(a)

		1		0		0		0
		0		c		s		0
		0		-s		c		0
		0		0		0		1
		*/
		double s = sin(a);
		double c = cos(a);
		return Matrix4(
			1.0,	0.0,	0.0,	0.0,
			0.0,	c,		s,		0.0,
			0.0,	-s,	c,		0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 rotateX(double s, double c)
	{
		/*
		let s = sin(a)
			 c = cos(a)

		1		0		0		0
		0		c		s		0
		0		-s		c		0
		0		0		0		1
		*/
		return Matrix4(
			1.0,	0.0,	0.0,	0.0,
			0.0,	c,		s,		0.0,
			0.0,	-s,	c,		0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 py_rotateXSinCos(double s, double c)
	{
		return rotateX( s, c );
	}

	inline static Matrix4 py_rotateX(double a)
	{
		return rotateX( a );
	}


	inline static Matrix4 rotateX90()
	{
		/*
		1		0		0		0
		0		0		1		0
		0		-1		0		0
		0		0		0		1
		*/
		return Matrix4(
			1.0,	0.0,	0.0,	0.0,
			0.0,	0.0,	-1.0,	0.0,
			0.0,	1.0,	0.0,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 rotateX180()
	{
		/*
		1		0		0		0
		0		-1		0		0
		0		0		-1		0
		0		0		0		1
		*/
		return Matrix4(
			1.0,	0.0,	0.0,	0.0,
			0.0,	-1.0,	0.0,	0.0,
			0.0,	0.0,	-1.0,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 rotateX270()
	{
		/*
		1		0		0		0
		0		0		-1		0
		0		1		0		0
		0		0		0		1
		*/
		return Matrix4(
			1.0,	0.0,	0.0,	0.0,
			0.0,	0.0,	-1.0,	0.0,
			0.0,	1.0,	0.0,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}



	inline static Matrix4 rotateY(double a)
	{
		/*
		let s = sin(a)
			 c = cos(a)

		c		0		-s		0
		0		1		0		0
		s		0		c		0
		0		0		0		1
		*/
		double s = sin(a);
		double c = cos(a);
		return Matrix4(
			c,		0.0,	-s,	0.0,
			0.0,	1.0,	0.0,	0.0,
			s,		0.0,	c,		0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 rotateY(double s, double c)
	{
		/*
		let s = sin(a)
			 c = cos(a)

		c		0		-s		0
		0		1		0		0
		s		0		c		0
		0		0		0		1
		*/
		return Matrix4(
			c,		0.0,	-s,	0.0,
			0.0,	1.0,	0.0,	0.0,
			s,		0.0,	c,		0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 py_rotateYSinCos(double s, double c)
	{
		return rotateY( s, c );
	}

	inline static Matrix4 py_rotateY(double a)
	{
		return rotateY( a );
	}

	inline static Matrix4 rotateY90()
	{
		/*
		0		0		-1		0
		0		1		0		0
		1		0		0		0
		0		0		0		1
		*/
		return Matrix4(
			0.0,	0.0,	-1.0,	0.0,
			0.0,	1.0,	0.0,	0.0,
			1.0,	0.0,	0.0,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 rotateY180()
	{
		/*
		-1		0		0		0
		0		1		0		0
		0		0		-1		0
		0		0		0		1
		*/
		return Matrix4(
			-1.0,	0.0,	0.0,	0.0,
			0.0,	1.0,	0.0,	0.0,
			0.0,	0.0,	-1.0,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 rotateY270()
	{
		/*
		0		0		1		0
		0		1		0		0
		-1		0		0		0
		0		0		0		1
		*/
		return Matrix4(
			0.0,	0.0,	1.0,	0.0,
			0.0,	1.0,	0.0,	0.0,
			-1.0,	0.0,	0.0,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}



	inline static Matrix4 rotateZ(double a)
	{
		/*
		let s = sin(a)
			 c = cos(a)

		c		s		0		0
		-s		c		0		0
		0		0		1		0
		0		0		0		1
		*/
		double s = sin(a);
		double c = cos(a);
		return Matrix4(
			c,		s,		0.0,	0.0,
			-s,	c,		0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 rotateZ(double s, double c)
	{
		/*
		let s = sin(a)
			 c = cos(a)

		c		s		0		0
		-s		c		0		0
		0		0		1		0
		0		0		0		1
		*/
		return Matrix4(
			c,		s,		0.0,	0.0,
			-s,	c,		0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 py_rotateZSinCos(double s, double c)
	{
		return rotateZ( s, c );
	}

	inline static Matrix4 py_rotateZ(double a)
	{
		return rotateZ( a );
	}

	inline static Matrix4 rotateZ90()
	{
		/*
		0		1		0		0
		-1		0		0		0
		0		0		1		0
		0		0		0		1
		*/
		return Matrix4(
			0.0,	1.0,	0.0,	0.0,
			-1.0,	0.0,	0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 rotateZ180()
	{
		/*
		-1		0		0		0
		0		-1		0		0
		0		0		1		0
		0		0		0		1
		*/
		return Matrix4(
			-1.0,	0.0,	0.0,	0.0,
			0.0,	-1.0,	0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}

	inline static Matrix4 rotateZ270()
	{
		/*
		0		-1		0		0
		1		0		0		0
		0		0		1		0
		0		0		0		1
		*/
		return Matrix4(
			0.0,	-1.0,	0.0,	0.0,
			1.0,	0.0,	0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			0.0,	0.0,	0.0,	1.0);
	}



	inline static Matrix4 rotate(Axis axis, double a)
	{
		switch( axis )
		{
		case AXIS_X:
			return Matrix4::rotateX( a );
			break;
		case AXIS_NEGATIVE_X:
			return Matrix4::rotateX( -a );
			break;
		case AXIS_Y:
			return Matrix4::rotateY( a );
			break;
		case AXIS_NEGATIVE_Y:
			return Matrix4::rotateY( -a );
			break;
		case AXIS_Z:
			return Matrix4::rotateZ( a );
			break;
		case AXIS_NEGATIVE_Z:
			return Matrix4::rotateZ( -a );
			break;
		default:
			return Matrix4();
			break;
		}
	}

	inline static Matrix4 py_rotateAxis(Axis axis, double a)
	{
		return rotate( axis, a );
	}

	inline static Matrix4 rotate(const Vector3 &axis, double a)
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
		1 - 2yy - 2zz		2xy + 2wz			2xz - 2wy			0
		2xy - 2wz			1 - 2xx - 2zz		2yz + 2wx			0
		2xz + 2wy			2yz - 2wx			1 - 2xx - 2yy		0
		0						0						0						1
		*/
		return Matrix4(
			1.0 - (yy + zz),	xy + wz,				xz - wy,				0.0,
			xy - wz,				1.0 - (xx + zz),	yz + wx,				0.0,
			xz + wy,				yz - wx,				1.0 - (xx + yy),	0.0,
			0.0,					0.0,					0.0,					1.0);
	}

	inline static Matrix4 py_rotateAxisVector(const Vector3 &axis, double a)
	{
		return rotate( axis, a );
	}

	inline static Matrix4 rotate(const EulerRotation &euler)
	{
		Quaternion q = Quaternion::rotate( euler );
		return Matrix4::rotate( q );
	}

	inline static Matrix4 py_rotateEuler(const EulerRotation &euler)
	{
		return rotate( euler );
	}

	inline static Matrix4 rotate(const Axes3 &axes, const EulerRotation &euler)
	{
		Quaternion q = Quaternion::rotate( axes, euler );
		return Matrix4::rotate( q );
	}

	inline static Matrix4 py_rotateEulerInAxes(const Axes3 &axes, const EulerRotation &euler)
	{
		return rotate( axes, euler );
	}

	inline static Matrix4 rotate(const Quaternion &q)
	{
	   double xx, yy, zz, xy, xz, yz, wx, wy, wz;

	   xx = q.x * q.x * 2.0;	yy = q.y * q.y * 2.0;	zz = q.z * q.z * 2.0;
		xy = q.x * q.y * 2.0;	xz = q.x * q.z * 2.0;	yz = q.y * q.z * 2.0;
	   wx = q.w * q.x * 2.0;	wy = q.w * q.y * 2.0;	wz = q.w * q.z * 2.0;


		// matrix:
		// 1 - 2yy - 2zz		2xy + 2wz			2xz - 2wy			0
		// 2xy - 2wz			1 - 2xx - 2zz		2yz + 2wx			0
		// 2xz + 2wy			2yz - 2wx			1 - 2xx - 2yy		0
		// 0						0						0						1

		return Matrix4(
			1.0 - (yy + zz),	xy + wz,				xz - wy,				0.0,
			xy - wz,				1.0 - (xx + zz),	yz + wx,				0.0,
			xz + wy,				yz - wx,				1.0 - (xx + yy),	0.0,
			0.0,					0.0,					0.0,					1.0);
	}

	inline static Matrix4 py_rotateQuaternion(const Quaternion &q)
	{
		return rotate( q );
	}



	inline static Matrix4 rotate90(Axis axis)
	{
		switch( axis )
		{
		case AXIS_X:
			return Matrix4::rotateX90();
			break;
		case AXIS_NEGATIVE_X:
			return Matrix4::rotateX270();
			break;
		case AXIS_Y:
			return Matrix4::rotateY90();
			break;
		case AXIS_NEGATIVE_Y:
			return Matrix4::rotateY270();
			break;
		case AXIS_Z:
			return Matrix4::rotateZ90();
			break;
		case AXIS_NEGATIVE_Z:
			return Matrix4::rotateZ270();
			break;
		default:
			return Matrix4();
			break;
		}
	}

	inline static Matrix4 rotate180(Axis axis)
	{
		switch( axis )
		{
		case AXIS_X:
			return Matrix4::rotateX180();
			break;
		case AXIS_NEGATIVE_X:
			return Matrix4::rotateX180();
			break;
		case AXIS_Y:
			return Matrix4::rotateY180();
			break;
		case AXIS_NEGATIVE_Y:
			return Matrix4::rotateY180();
			break;
		case AXIS_Z:
			return Matrix4::rotateZ180();
			break;
		case AXIS_NEGATIVE_Z:
			return Matrix4::rotateZ180();
			break;
		default:
			return Matrix4();
			break;
		}
	}

	inline static Matrix4 rotate270(Axis axis)
	{
		switch( axis )
		{
		case AXIS_X:
			return Matrix4::rotateX270();
			break;
		case AXIS_NEGATIVE_X:
			return Matrix4::rotateX90();
			break;
		case AXIS_Y:
			return Matrix4::rotateY270();
			break;
		case AXIS_NEGATIVE_Y:
			return Matrix4::rotateY90();
			break;
		case AXIS_Z:
			return Matrix4::rotateZ270();
			break;
		case AXIS_NEGATIVE_Z:
			return Matrix4::rotateZ90();
			break;
		default:
			return Matrix4();
			break;
		}
	}





	inline static Matrix4 shearX(double yShear, double zShear)
	{
		/*
		1			sy/sx		sz/sx		0
		0			1			0			0
		0			0			1			0
		0			0			0			1
		*/
		return Matrix4(
				1.0,			yShear,		zShear,		0.0,
				0.0,			1.0,			0.0,			0.0,
				0.0,			0.0,			1.0,			0.0,
				0.0,			0.0,			0.0,			1.0);
	}

	inline static Matrix4 shearY(double xShear, double zShear)
	{
		/*
		1			0			0			0
		sx/sy		1			sz/sy		0
		0			0			1			0
		0			0			0			1
		*/
		return Matrix4(
				1.0,			0.0,			0.0,			0.0,
				xShear,		1.0,			zShear,		0.0,
				0.0,			0.0,			1.0,			0.0,
				0.0,			0.0,			0.0,			1.0);
	}

	inline static Matrix4 shearZ(double xShear, double yShear)
	{
		/*
		1			0			0			0
		0			1			0			0
		sx/sz		sy/sz		1			0
		0			0			0			1
		*/
		return Matrix4(
				1.0,			0.0,			0.0,			0.0,
				0.0,			1.0,			0.0,			0.0,
				xShear,		yShear,		1.0,			0.0,
				0.0,			0.0,			0.0,			1.0);
	}

	inline static Matrix4 shear(const Vector3 &shearAxis, const Vector3 &shearVector)
	{
		double i = shearAxis.x;			// [1,0,0] (dot) shearAxis
		double j = shearAxis.y;			// [0,1,0] (dot) shearAxis
		double k = shearAxis.z;			// [0,0,1] (dot) shearAxis

		//compute the base vectors
		Vector3 baseI = Vector3( 1.0, 0.0, 0.0 )  +  shearVector * i;
		Vector3 baseJ = Vector3( 0.0, 1.0, 0.0 )  +  shearVector * j;
		Vector3 baseK = Vector3( 0.0, 0.0, 1.0 )  +  shearVector * k;

		return Matrix4( baseI, baseJ, baseK );
	}

	inline static Matrix4 py_shear3(const Vector3 &shearAxis, const Vector3 &shearVector)
	{
		return shear( shearAxis, shearVector );
	}

	inline static Matrix4 shear(const Vector2 &shearAxis, const Vector2 &shearVector)
	{
		double i = shearAxis.x;			// [1,0] (dot) shearAxis
		double j = shearAxis.y;			// [0,1] (dot) shearAxis

		//compute the base vectors
		Vector2 baseI = Vector2( 1.0, 0.0 )  +  shearVector * i;
		Vector2 baseJ = Vector2( 0.0, 1.0 )  +  shearVector * j;

		return Matrix4( baseI, baseJ );
	}

	inline static Matrix4 py_shear2(const Vector2 &shearAxis, const Vector2 &shearVector)
	{
		return shear( shearAxis, shearVector );
	}





	inline static Matrix4 reflect(const Vector3 &n)
	{
		double nx = -2.0 * n.x,   ny = -2.0 * n.y,   nz = -2.0 * n.z;

			/*
			1 - 2xx		-2xy			-2xz			0
			-2yx			1 - 2yy		-2yz			0
			-2zx			-2zy			1 - 2zz		0
			0				0				0				1
			*/
		return Matrix4(
			1.0 + nx * n.x,	nx * n.y, 			nx * n.z,			0.0,
			ny * n.x,			1.0 + ny * n.y,	ny * n.z,			0.0,
			nz * n.x,			nz * n.y,			1.0 + nz * n.z,	0.0,
			0.0,					0.0,					0.0,					1.0);
	}

	inline static Matrix4 py_reflectNormal(const Vector3 &n)
	{
		return reflect( n );
	}

	inline static Matrix4 reflect(const Plane &p)
	{
		double nx = -2 * p.n.x,   ny = -2 * p.n.y,   nz = -2 * p.n.z;
		/*
		1 - 2xx		-2xy			-2xz			0
		-2yx			1 - 2yy		-2yz			0
		-2zx			-2zy			1 - 2zz		0
		2dx			2dy			2dz			1
		*/
		return Matrix4(
			1.0 + nx * p.n.x,	nx * p.n.y, 		nx * p.n.z,			0.0,
			ny * p.n.x,			1.0 + ny * p.n.y,	ny * p.n.z,			0.0,
			nz * p.n.x,			nz * p.n.y,			1.0 + nz * p.n.z,	0.0,
			-nx * p.d,			-ny * p.d,			-nz * p.d,			1.0);
	}

	inline static Matrix4 py_reflectPlane(const Plane &p)
	{
		return reflect( p );
	}






	inline static Matrix4 spaceToSpace(const Vector3 &i, const Vector3 &j, const Vector3 &k, const Vector3 &u, const Vector3 &v, const Vector3 &w)
	{
		//inverse of matrix ijk multiplied by matrix uvw
		return Matrix4( i, j, k ).inverse()  *  Matrix4( u, v, w );
	}

	inline static Matrix4 py_spaceToSpace3(const Vector3 &i, const Vector3 &j, const Vector3 &k, const Vector3 &u, const Vector3 &v, const Vector3 &w)
	{
		return spaceToSpace( i, j, k, u, v, w );
	}

	inline static Matrix4 spaceToSpace(const Vector2 &i, const Vector2 &j, const Vector2 &u, const Vector2 &v)
	{
		//inverse of matrix ij multiplied by matrix uv
		return Matrix4( i, j ).inverse()  *  Matrix4( u, v );
	}

	inline static Matrix4 py_spaceToSpace2(const Vector2 &i, const Vector2 &j, const Vector2 &u, const Vector2 &v)
	{
		return spaceToSpace( i, j, u, v );
	}

	inline static Matrix4 spaceToSpace(const Triangle3 &t0, const Triangle3 &t1)
	{
		Vector3 i = t0.b - t0.a;
		Vector3 j = t0.c - t0.a;
		Vector3 k = i.cross( j );

		Vector3 u = t1.b - t1.a;
		Vector3 v = t1.c - t1.a;
		Vector3 w = u.cross( v );

		//inverse of matric ijk multiplied by matrix uvw
		return Matrix4( t0.a, i, j, k ).inverse()  *  Matrix4( t1.a, u, v, w );
	}

	inline static Matrix4 py_spaceToSpaceTri3(const Triangle3 &t0, const Triangle3 &t1)
	{
		return spaceToSpace( t0, t1 );
	}

	inline static Matrix4 spaceToSpace(const Triangle2 &t0, const Triangle2 &t1)
	{
		Vector2 i = t0.b - t0.a;
		Vector2 j = t0.c - t0.a;

		Vector2 u = t1.b - t1.a;
		Vector2 v = t1.c - t1.a;

		//inverse of matric ijk multiplied by matrix uvw
		return Matrix4( t0.a, i, j ).inverse()  *  Matrix4( t1.a, u, v );
	}

	inline static Matrix4 py_spaceToSpaceTri2(const Triangle2 &t0, const Triangle2 &t1)
	{
		return spaceToSpace( t0, t1 );
	}




	inline static Matrix4 axesMatrix(const Axes3 &axes)
	{
		return Matrix4( axes.i, axes.j, axes.k );
	}

	inline static Matrix4 axesToAxesMatrix(const Axes3 &from, const Axes3 &to)
	{
		return Matrix4::spaceToSpace( from.i, from.j, from.k,
									to.i, to.j, to.k );
	}




	inline static Matrix4 axisToAxis(const Vector3 &src, const Vector3 &dst)
	{
		//find the angle between the vectors
		//and generate a rotation matrix, rotating that amount, where the axis
		//of rotation is the vector that is perpendicular to src and dst
		double cosAngle = src.dot( dst );

		//check if src and dst are the same vector (include margin of error)
		if ( cosAngle  >  1.0 - EPSILON )
		{
			//if so, result is identity matrix
			return Matrix4();
		}
		else if ( cosAngle  <  -1.0 + EPSILON )		//check if they oppose
		{
			//get a vector perpendicular to the src vector
			Vector3 axis = src.perpendicularVector();
			axis.normalise();

			//rotate around this by 180 degrees
			return Matrix4::rotate( axis, M_PI * 2.0 );
		}
		else
		{
			//calculate rotation axis
			Vector3 axis = src.cross( dst );
			axis.normalise();
			//calculate angle
			double angle = acos( cosAngle );
			//generate the matrix
			return Matrix4::rotate( axis, angle );
		}
	}

	inline static Matrix4 py_axisToAxisVector(const Vector3 &src, const Vector3 &dst)
	{
		return axisToAxis( src, dst );
	}

	inline static Matrix4 axisToAxis(Axis src, Axis dst)
	{
		//s is a scale factor, used to reduce code size, it is used to
		//invert the effect of the matrix in the case of a negative axis
		double s = 1.0;

		switch( src )
		{
		case AXIS_NEGATIVE_X:
			s = -1.0;
		case AXIS_X:
			switch( dst )
			{
			case AXIS_NEGATIVE_X:
				s = -s;
			case AXIS_X:
				return Matrix4(
						s,		0.0,	0.0,	0.0,
						0.0,	1.0,	0.0,	0.0,
						0.0,	0.0,	s,		0.0,
						0.0,	0.0,	0.0,	1.0 );
				break;
			case AXIS_NEGATIVE_Y:
				s = -s;
			case AXIS_Y:
				return Matrix4(
						0.0,	s,		0.0,	0.0,
						-s,	0.0,	0.0,	0.0,
						0.0,	0.0,	1.0,	0.0,
						0.0,	0.0,	0.0,	1.0 );
				break;
			case AXIS_NEGATIVE_Z:
				s = -s;
			case AXIS_Z:
				return Matrix4(
						0.0,	0.0,	s,		0.0,
						0.0,	1.0,	0.0,	0.0,
						-s,	0.0,	0.0,	0.0,
						0.0,	0.0,	0.0,	1.0 );
				break;
			default:
				return Matrix4();
			}
			break;

		case AXIS_NEGATIVE_Y:
			s = -1.0;
		case AXIS_Y:
			switch( dst )
			{
			case AXIS_NEGATIVE_X:
				s = -s;
			case AXIS_X:
				return Matrix4(
						0.0,	-s,	0.0,	0.0,
						s,		0.0,	0.0,	0.0,
						0.0,	0.0,	1.0,	0.0,
						0.0,	0.0,	0.0,	1.0 );
				break;
			case AXIS_NEGATIVE_Y:
				s = -s;
			case AXIS_Y:
				return Matrix4(
						1.0,	0.0,	0.0,	0.0,
						0.0,	s,		0.0,	0.0,
						0.0,	0.0,	s,		0.0,
						0.0,	0.0,	0.0,	1.0 );
				break;
			case AXIS_NEGATIVE_Z:
				s = -s;
			case AXIS_Z:
				return Matrix4(
						1.0,	0.0,	0.0,	0.0,
						0.0,	0.0,	s,		0.0,
						0.0,	-s,	0.0,	0.0,
						0.0,	0.0,	0.0,	1.0 );
				break;
			default:
				return Matrix4();
			}
			break;

		case AXIS_NEGATIVE_Z:
			s = -1.0;
		case AXIS_Z:
			switch( dst )
			{
			case AXIS_NEGATIVE_X:
				s = -s;
			case AXIS_X:
				return Matrix4(
						0.0,	0.0,	-s,	0.0,
						0.0,	1.0,	0.0,	0.0,
						s,		0.0,	0.0,	0.0,
						0.0,	0.0,	0.0,	1.0 );
				break;
			case AXIS_NEGATIVE_Y:
				s = -s;
			case AXIS_Y:
				return Matrix4(
						1.0,	0.0,	0.0,	0.0,
						0.0,	0.0,	-s,	0.0,
						0.0,	s,		0.0,	0.0,
						0.0,	0.0,	0.0,	1.0 );
				break;
			case AXIS_NEGATIVE_Z:
				s = -s;
			case AXIS_Z:
				return Matrix4(
						s,		0.0,	0.0,	0.0,
						0.0,	1.0,	0.0,	0.0,
						0.0,	0.0,	s,		0.0,
						0.0,	0.0,	0.0,	1.0 );
				break;
			default:
				return Matrix4();
			}
			break;
		default:
			return Matrix4();
		}
	}

	inline static Matrix4 py_axisToAxis(Axis src, Axis dst)
	{
		return axisToAxis( src, dst );
	}





	inline static Matrix4 lookat(const Point3 &position, const Point3 &lookatPoint, const Vector3 &up)
	{
		Vector3 lookDirection = lookatPoint - position;
		Vector3 right = lookDirection.cross( up );

		if ( right.sqrLength() < SQR_EPSILON )
		{
			right = Vector3( 1.0, 0.0, 0.0 );
		}

		Vector3 realUp = right.cross( lookDirection );

		lookDirection.normalise();
		right.normalise();
		realUp.normalise();

		return Matrix4( right, realUp, -lookDirection ) *
				 Matrix4::translate( position.toVector3() );
	}

	inline static Matrix4 py_lookatPoints(const Point3 &position, const Point3 &lookatPoint, const Vector3 &up)
	{
		return lookat( position, lookatPoint, up );
	}

	inline static Matrix4 lookat(const Vector3 &lookDirection, const Vector3 &up)
	{
		Vector3 right = lookDirection.cross( up );

		if ( right.sqrLength() < SQR_EPSILON )
		{
			right = Vector3( 1.0, 0.0, 0.0 );
		}

		Vector3 realUp = right.cross( lookDirection );

		right.normalise();
		realUp.normalise();

		return Matrix4( right, realUp, -lookDirection.getNormalised() );
	}

	inline static Matrix4 py_lookatDirections(const Vector3 &lookDirection, const Vector3 &up)
	{
		return lookat( lookDirection, up );
	}





	inline static Matrix4 perspective(double nearClip, double farClip)
	{
		/*
		z -> in range (-n,-f)						(input)
		w = 1.0											(input)

		z' = az + b										(transformation)
		w' = -z											(transformation)
		z'' = ( z' / w' )  ->  in range (0,1)  (homogeneous divide)
		a = far  /  ( near - far )
		b = far * near  /  ( near - far )

		1		0		0		0
		0		1		0		0
		0		0		a		-1
		0		0		b		0
		*/
		double a = farClip  /  ( nearClip - farClip );
		double b = farClip * nearClip  /  ( nearClip - farClip );
		return Matrix4(
			1.0,		0.0,		0.0,		0.0,
			0.0,		1.0,		0.0,		0.0,
			0.0,		0.0,		a,			-1.0,
			0.0,		0.0,		b,			0.0 );
	}





	inline static Matrix4 orthographic(double nearClip, double farClip)
	{
		/*
		z -> in range (-n,-f)						(input)
		w = 1.0											(input)

		z' = az + b										(transformation)
		w' = 1.0											(transformation)
		z'' = z' / w'  =  z' / 1.0  =  z' z'  ->  in range (0,1)  (homogeneous divide)
		a = -1.0  /  ( far - near )
		b = -near  /  ( far - near )

		1		0		0		0
		0		1		0		0
		0		0		a		0
		0		0		b		1
		*/
		double a = 1.0  /  ( nearClip - farClip );
		double b = nearClip  /  ( nearClip - farClip );
		return Matrix4(
			1.0,		0.0,		0.0,		0.0,
			0.0,		1.0,		0.0,		0.0,
			0.0,		0.0,		a,			0.0,
			0.0,		0.0,		b,			1.0 );
	}





	inline static Matrix4 camera(double fovY, double aspectRatio)
	{
		double x, y;

		y = tan( fovY * 0.5 );
		x = y * aspectRatio;

		/*
		1/x	0		0		0
		0		1/y	0		0
		0		0		1		0
		0		0		0		1
		*/
		return Matrix4(
			1.0 / x,	0.0,		0.0,		0.0,
			0.0,		1.0 / y,	0.0,		0.0,
			0.0,		0.0,		1.0,		0.0,
			0.0,		0.0,		0.0,		1.0 );
	}





	inline static Matrix4 screen(double width, double height)
	{
		double halfW = width * 0.5;
		double halfH = height * 0.5;
		/*
		w/2	0		0		0
		0		h/2	0		0
		0		0		1		0
		w/2	h/2	0		1
		*/
		return Matrix4(
			halfW,	0.0,		0.0,		0.0,
			0.0,		halfH,	0.0,		0.0,
			0.0,		0.0,		1.0,		0.0,
			halfW,	halfH,	0.0,		1.0 );
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


	//homogeneous transforms: perspective divide
	inline Point3 transformHomogeneous(const Point3 &p3) const
	{
		Point4 p = Point4( p3 );

		double x =	p.x * iM00  +  p.y * iM10  +  p.z * iM20  +  p.w * iM30;
		double y =	p.x * iM01  +  p.y * iM11  +  p.z * iM21  +  p.w * iM31;
		double z =	p.x * iM02  +  p.y * iM12  +  p.z * iM22  +  p.w * iM32;
		p.w =			p.x * iM03  +  p.y * iM13  +  p.z * iM23  +  p.w * iM33;
		p.x = x;
		p.y = y;
		p.z = z;

		return p.convertToPoint3();
	}

	inline Point3 transformHomogeneous(const Point2 &p2, double z) const
	{
		return transformHomogeneous( Point3( p2, z ) );
	}

	inline Point2 transformHomogeneous2d(const Point3 &p3) const
	{
		return transformHomogeneous( p3 ).toPoint2();
	}


	inline Segment3 transformHomogeneous(const Segment3 &seg3) const
	{
		return Segment3( transformHomogeneous( seg3.a ), transformHomogeneous( seg3.b ) );
	}

	inline Segment2 transformHomogeneous2d(const Segment3 &seg3) const
	{
		return Segment2( transformHomogeneous2d( seg3.a ), transformHomogeneous2d( seg3.b ) );
	}


	inline Triangle3 transformHomogeneous(const Triangle3 &t3) const
	{
		return Triangle3( transformHomogeneous( t3.a ),
								transformHomogeneous( t3.b ),
								transformHomogeneous( t3.c ) );
	}

	inline Triangle2 transformHomogeneous2d(const Triangle3 &t3) const
	{
		return Triangle2( transformHomogeneous2d( t3.a ),
								transformHomogeneous2d( t3.b ),
								transformHomogeneous2d( t3.c ) );
	}


	inline Point3 py_transformHomogeneousPoint3(const Point3 &p) const
	{
		return transformHomogeneous( p );
	}

	inline Point3 py_transformHomogeneousPoint2_z(const Point2 &p, double z) const
	{
		return transformHomogeneous( p, z );
	}

	inline Point2 py_transformHomogeneous2dPoint3(const Point3 &p) const
	{
		return transformHomogeneous2d( p );
	}


	inline Segment3 py_transformHomogeneousSegment(const Segment3 &s) const
	{
		return transformHomogeneous( s );
	}

	inline Segment2 py_transformHomogeneous2dSegment(const Segment3 &s) const
	{
		return transformHomogeneous2d( s );
	}


	inline Triangle3 py_transformHomogeneousTriangle(const Triangle3 &t) const
	{
		return transformHomogeneous( t );
	}

	inline Triangle2 py_transformHomogeneous2dTriangle(const Triangle3 &t) const
	{
		return transformHomogeneous2d( t );
	}



	//transform p, getting result.z only
	inline double transformZ(const Point3 &p) const
	{
		return p.x * iM02 +	p.y * iM12 +	p.z * iM22 +	iM32;
	}

	//transform p with homogeneous divide, getting result.z only
	inline double transformZHomogeneous(const Point3 &p) const
	{
		double z = p.x * iM02  +  p.y * iM12  +  p.z * iM22  +  iM32;
		double w = p.x * iM03  +  p.y * iM13  +  p.z * iM23  +  iM33;
		return z / w;
	}
};


//multiply Point2f by a matrix; transform the point
inline Point2f operator*(const Point2f &p, const Matrix4 &m)
{
	return Point2f(
		p.x * m.iM00  +  p.y * m.iM10  +  m.iM30,
		p.x * m.iM01  +  p.y * m.iM11  +  m.iM31 );
}

//multiply Point2 by a matrix; transform the point
inline Point2 operator*(const Point2 &p, const Matrix4 &m)
{
	return Point2(
		p.x * m.iM00  +  p.y * m.iM10  +  m.iM30,
		p.x * m.iM01  +  p.y * m.iM11  +  m.iM31 );
}

//multiply Point3f by a matrix; transform the point
inline Point3f operator*(const Point3f &p, const Matrix4 &m)
{
	return Point3f(
		p.x * m.iM00  +  p.y * m.iM10  +  p.z * m.iM20  +  m.iM30,
		p.x * m.iM01  +  p.y * m.iM11  +  p.z * m.iM21  +  m.iM31,
		p.x * m.iM02  +  p.y * m.iM12  +  p.z * m.iM22  +  m.iM32 );
}

//multiply Point3 by a matrix; transform the point
inline Point3 operator*(const Point3 &p, const Matrix4 &m)
{
	return Point3(
		p.x * m.iM00  +  p.y * m.iM10  +  p.z * m.iM20  +  m.iM30,
		p.x * m.iM01  +  p.y * m.iM11  +  p.z * m.iM21  +  m.iM31,
		p.x * m.iM02  +  p.y * m.iM12  +  p.z * m.iM22  +  m.iM32 );
}

//multiply Point4 by a matrix; transform the point
inline Point4 operator*(const Point4 &p, const Matrix4 &m)
{
	return Point4(
		p.x * m.iM00  +  p.y * m.iM10  +  p.z * m.iM20  +  p.w * m.iM30,
		p.x * m.iM01  +  p.y * m.iM11  +  p.z * m.iM21  +  p.w * m.iM31,
		p.x * m.iM02  +  p.y * m.iM12  +  p.z * m.iM22  +  p.w * m.iM32,
		p.x * m.iM03  +  p.y * m.iM13  +  p.z * m.iM23  +  p.w * m.iM33 );
}

//multiply Vector2f by a matrix; transform the vector
inline Vector2f operator*(const Vector2f &v, const Matrix4 &m)
{
	return Vector2f(
		v.x * m.iM00  +  v.y * m.iM10,
		v.x * m.iM01  +  v.y * m.iM11 );
}

//multiply Vector2 by a matrix; transform the vector
inline Vector2 operator*(const Vector2 &v, const Matrix4 &m)
{
	return Vector2(
		v.x * m.iM00  +  v.y * m.iM10,
		v.x * m.iM01  +  v.y * m.iM11 );
}

//multiply Vector3f by a matrix; transform the vector
inline Vector3f operator*(const Vector3f &v, const Matrix4 &m)
{
	return Vector3f(
		v.x * m.iM00  +  v.y * m.iM10  +  v.z * m.iM20,
		v.x * m.iM01  +  v.y * m.iM11  +  v.z * m.iM21,
		v.x * m.iM02  +  v.y * m.iM12  +  v.z * m.iM22 );
}

//multiply Vector3 by a matrix; transform the vector
inline Vector3 operator*(const Vector3 &v, const Matrix4 &m)
{
	return Vector3(
		v.x * m.iM00  +  v.y * m.iM10  +  v.z * m.iM20,
		v.x * m.iM01  +  v.y * m.iM11  +  v.z * m.iM21,
		v.x * m.iM02  +  v.y * m.iM12  +  v.z * m.iM22 );
}

//multiply Plane by a matrix
inline Plane operator*(const Plane &p, const Matrix4 &m)
{
	Point3 a = p.getPointOnPlane()  *  m;
	Vector3 n = ( p.n * m ).getNormalised();
	return Plane( n, a );
}

//multiply Segment3 by a matrix
inline Segment3 operator*(const Segment3 &seg, const Matrix4 &m)
{
	return Segment3( seg.a * m, seg.b * m );
}

//multiply Triangle3 by a matrix
inline Triangle3 operator*(const Triangle3 &t, const Matrix4 &m)
{
	return Triangle3( t.a * m, t.b * m, t.c * m );
}

//multiply Axes3 by a matrix
inline Axes3 operator*(const Axes3 &a, const Matrix4 &m)
{
	return Axes3( ( a.i * m ).getNormalised(),
					  ( a.j * m ).getNormalised(),
					  ( a.k * m ).getNormalised() );
}




inline void operator*=(Point2f &p, const Matrix4 &m)
{
	float x	=	p.x * m.iM00  +  p.y * m.iM10  +  m.iM30;
	p.y		=	p.x * m.iM01  +  p.y * m.iM11  +  m.iM31;
	p.x = x;
}

inline void operator*=(Point2 &p, const Matrix4 &m)
{
	double x =	p.x * m.iM00  +  p.y * m.iM10  +  m.iM30;
	p.y		=	p.x * m.iM01  +  p.y * m.iM11  +  m.iM31;
	p.x = x;
}

inline void operator*=(Point3f &p, const Matrix4 &m)
{
	float x =	p.x * m.iM00  +  p.y * m.iM10  +  p.z * m.iM20  +  m.iM30;
	float y =	p.x * m.iM01  +  p.y * m.iM11  +  p.z * m.iM21  +  m.iM31;
	p.z =			p.x * m.iM02  +  p.y * m.iM12  +  p.z * m.iM22  +  m.iM32;
	p.x = x;
	p.y = y;
}

inline void operator*=(Point3 &p, const Matrix4 &m)
{
	double x =	p.x * m.iM00  +  p.y * m.iM10  +  p.z * m.iM20  +  m.iM30;
	double y =	p.x * m.iM01  +  p.y * m.iM11  +  p.z * m.iM21  +  m.iM31;
	p.z =			p.x * m.iM02  +  p.y * m.iM12  +  p.z * m.iM22  +  m.iM32;
	p.x = x;
	p.y = y;
}

inline void operator*=(Point4 &p, const Matrix4 &m)
{
	double x =	p.x * m.iM00  +  p.y * m.iM10  +  p.z * m.iM20  +  p.w * m.iM30;
	double y =	p.x * m.iM01  +  p.y * m.iM11  +  p.z * m.iM21  +  p.w * m.iM31;
	double z =	p.x * m.iM02  +  p.y * m.iM12  +  p.z * m.iM22  +  p.w * m.iM32;
	p.w =			p.x * m.iM03  +  p.y * m.iM13  +  p.z * m.iM23  +  p.w * m.iM33;
	p.x = x;
	p.y = y;
	p.z = z;
}

inline void operator*=(Vector2f &v, const Matrix4 &m)
{
	float x =	v.x * m.iM00  +  v.y * m.iM10  +  m.iM30;
	v.y		=	v.x * m.iM01  +  v.y * m.iM11  +  m.iM31;
	v.x = x;
}

inline void operator*=(Vector2 &v, const Matrix4 &m)
{
	double x =	v.x * m.iM00  +  v.y * m.iM10  +  m.iM30;
	v.y		=	v.x * m.iM01  +  v.y * m.iM11  +  m.iM31;
	v.x = x;
}

inline void operator*=(Vector3f &v, const Matrix4 &m)
{
	float x =	v.x * m.iM00  +  v.y * m.iM10  +  v.z * m.iM20;
	float y =	v.x * m.iM01  +  v.y * m.iM11  +  v.z * m.iM21;
	v.z =			v.x * m.iM02  +  v.y * m.iM12  +  v.z * m.iM22;
	v.x = x;
	v.y = y;
}

inline void operator*=(Vector3 &v, const Matrix4 &m)
{
	double x =	v.x * m.iM00  +  v.y * m.iM10  +  v.z * m.iM20;
	double y =	v.x * m.iM01  +  v.y * m.iM11  +  v.z * m.iM21;
	v.z =			v.x * m.iM02  +  v.y * m.iM12  +  v.z * m.iM22;
	v.x = x;
	v.y = y;
}

inline void operator*=(Plane &p, const Matrix4 &m)
{
	Point3 pointOnPlane = p.getPointOnPlane();
	pointOnPlane *= m;
	p.n *= m;
	p.n.normalise();
	p.d = pointOnPlane.dot( p.n );
}

inline void operator*=(Segment3 &seg, const Matrix4 &m)
{
	seg.a *= m;
	seg.b *= m;
}

inline void operator*=(Triangle3 &t, const Matrix4 &m)
{
	t.a *= m;
	t.b *= m;
	t.c *= m;
}

inline void operator*=(Axes3 &a, const Matrix4 &m)
{
	a.i *= m;
	a.j *= m;
	a.k *= m;

	a.normalise();
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
#undef iMJ
#undef iMK
#undef iML
#undef iMM
#undef iMN
#undef iMO
#undef iMP

#undef iM00
#undef iM01
#undef iM02
#undef iM03
#undef iM10
#undef iM11
#undef iM12
#undef iM13
#undef iM20
#undef iM21
#undef iM22
#undef iM23
#undef iM30
#undef iM31
#undef iM32
#undef iM33



#endif
