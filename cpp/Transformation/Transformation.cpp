//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef TRANSFORMATION_CPP__
#define TRANSFORMATION_CPP__

#include <Transformation/Transformation.h>


#ifndef NULL
#define NULL 0
#endif



/*
*******************************************************************************
										Transformation
*******************************************************************************
*/

Transformation::Transformation()
{
	child = NULL;
}

Transformation::Transformation(const Transformation &c)
{
	if ( c.child == NULL )
	{
		child = NULL;
	}
	else
	{
		child = c.child->clone();
	}
}

Transformation::~Transformation()
{
	if ( child != NULL )
	{
		delete child;
	}
}


Point3 Transformation::transformPoint(const Point3 &p) const
{
	if ( child != NULL )
	{
		return child->transformPoint( xfPoint( p ) );
	}
	else
	{
		return xfPoint( p );
	}
}

Vector3 Transformation::transformVector(const Vector3 &v, const Point3 &at) const
{
	if ( child != NULL )
	{
		return child->transformVector( xfVector( v, at ), xfPoint( at ) );
	}
	else
	{
		return xfVector( v, at );
	}
}


void Transformation::chainTo(const Transformation *x)
{
	if ( child != NULL )
	{
		delete child;
	}

	child = x->clone();
}

Transformation* Transformation::clone() const
{
	return new Transformation( *this );
}


Point3 Transformation::xfPoint(const Point3 &p) const
{
	return p;
}

Vector3 Transformation::xfVector(const Vector3 &v, const Point3 &at) const
{
	return v;
}





#endif
