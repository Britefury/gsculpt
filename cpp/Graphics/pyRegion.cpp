//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYREGION_CPP__
#define PYREGION_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Graphics/Region.h>


void export_Region()
{
	class_<Region2d, bases<RTObject> >( "Region2d", init<>() );

	class_<RectangleRegion2d, bases<Region2d> >( "RectangleRegion2d", init<>() )
		.def( init<const BBox2 &>() )
		.def( init<const Point2 &, const Point2 &>() )
		.def_readwrite( "_box", &RectangleRegion2d::box );

	class_<CircleRegion2d, bases<Region2d> >( "CircleRegion2d", init<>() )
		.def( init<const Circle2 &>() )
		.def_readwrite( "_circle", &CircleRegion2d::circle );

	class_<PolylineRegion2d, bases<Region2d> >( "PolylineRegion2d", init<>() )
		.def( init<boost::python::list>() )
		.add_property( "_vertices", &PolylineRegion2d::py_getVertices, &PolylineRegion2d::py_setVertices );

	class_<LassoRegion2d, bases<Region2d> >( "LassoRegion2d", init<>() )
		.def( init<const Polygon2 &>() )
		.def_readwrite( "_lasso", &LassoRegion2d::lasso );

	class_<Region3d, bases<RTObject> >( "Region3d", init<>() )
		.def( "paint", &Region3d::paint );

	class_<ProjectedRegion3d, bases<Region3d> >( "ProjectedRegion3d", init<>() )
		.def( init<const Region2d &, const Projection &>() )
		.def_readwrite( "_projection", &ProjectedRegion3d::projection )
		.def( "_p_getRegion2d", &ProjectedRegion3d::getRegion2d, return_internal_reference<>() )
		.def( "_p_setRegion2d", &ProjectedRegion3d::setRegion2d );
}


#endif
