//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYCAMERAPROJECTION_CPP__
#define PYCAMERAPROJECTION_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Graphics/CameraProjection.h>


void export_CameraProjection()
{
	enum_<CameraProjection::CameraProjectionType>( "CameraProjectionType" )
		.value( "NONE", CameraProjection::NONE )
		.value( "PERSPECTIVE", CameraProjection::PERSPECTIVE )
		.value( "ORTHOGRAPHIC", CameraProjection::ORTHOGRAPHIC );

	class_<CameraProjection>( "CameraProjection", init<>() )
		.def( init<double, double>() )
		.def( "setProjectionParameters", &CameraProjection::setProjectionParameters )
		.def( "getProjectionType", &CameraProjection::getProjectionType )
		.def( "setScreenDimensions", &CameraProjection::setScreenDimensions )
		.def( "setClipFactors", &CameraProjection::setClipFactors )
		.def( "setFieldOfView", &CameraProjection::setFieldOfView )
		.def( "setCameraToTargetDistance", &CameraProjection::setCameraToTargetDistance )
		.def( "getScreenWidth", &CameraProjection::getScreenWidth )
		.def( "getScreenHeight", &CameraProjection::getScreenHeight )
		.def( "getNearClipDistance", &CameraProjection::getNearClipDistance )
		.def( "getFarClipDistance", &CameraProjection::getFarClipDistance )
		.def( "getOneToOneDistance", &CameraProjection::getOneToOneDistance )
		.def( "transformDistanceAcrossScreenToDistanceThruWorld", &CameraProjection::py_transformDistanceAcrossScreenToDistanceThruWorld )
		.def( "transformDistanceAcrossScreenToDistanceThruWorld", &CameraProjection::py_transformDistanceAcrossScreenToDistanceThruWorldAtDist )
		.def( "transformFractionalDistAcrossScreenToDistThruWorld", &CameraProjection::transformFractionalDistAcrossScreenToDistThruWorld )
		.def( "cameraZToScreenZ", &CameraProjection::cameraZToScreenZ )
		.def( "getFrustum", &CameraProjection::getFrustum, return_internal_reference<>() )
		.def( "getTopLeftCornerRay", &CameraProjection::getTopLeftCornerRay, return_internal_reference<>() )
		.def( "getTopRightCornerRay", &CameraProjection::getTopRightCornerRay, return_internal_reference<>() )
		.def( "getBottomLeftCornerRay", &CameraProjection::getBottomLeftCornerRay, return_internal_reference<>() )
		.def( "getBottomRightCornerRay", &CameraProjection::getBottomRightCornerRay, return_internal_reference<>() )
		.def( "getViewPlaneSize", &CameraProjection::getViewPlaneSize )
		.def( "setGLProjectionMatrix", &CameraProjection::setGLProjectionMatrix )
		.def( "getCameraToScreenMatrix", &CameraProjection::getCameraToScreenMatrix, return_internal_reference<>() )
		.def( "getScreenToCameraMatrix", &CameraProjection::getScreenToCameraMatrix, return_internal_reference<>() );

	class_<PerspectiveCameraProjection, bases<CameraProjection> >( "PerspectiveCameraProjection", init<>() )
		.def( init<double, double>() );

	class_<OrthographicCameraProjection, bases<CameraProjection> >( "OrthographicCameraProjection", init<>() )
		.def( init<double, double>() );
}


#endif
