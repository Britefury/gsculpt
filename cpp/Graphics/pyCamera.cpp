//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYCAMERA_CPP__
#define PYCAMERA_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Graphics/Camera.h>


void export_Camera()
{
	class_<Camera>( "Camera", init<double, double, double>() )
		.def( "setScreenDimensions", &Camera::setScreenDimensions )
		.def( "setFieldOfView", &Camera::setFieldOfView )
		.def( "getCameraPosition", &Camera::getCameraPosition, return_internal_reference<>() )
		.def( "setCameraPosition", &Camera::setCameraPosition, with_custodian_and_ward<1, 2>() )
		.def( "getCameraProjection", &Camera::getCameraProjection, return_internal_reference<>() )
		.def( "setCameraProjection", &Camera::setCameraProjection, with_custodian_and_ward<1, 2>() )
		.def( "oneToOne", &Camera::oneToOne )
		.def( "setGLProjectionMatrix", &Camera::setGLProjectionMatrix )
		.def( "setGLModelViewMatrix", &Camera::setGLModelViewMatrix )
		.def( "getFrustum", &Camera::getFrustum, return_internal_reference<>() )
		.def( "getProjection", &Camera::getProjection )
		.def( "transformScreenAtTargetDistanceToWorld", &Camera::transformScreenAtTargetDistanceToWorld )
		.def( "transformWorldToScreen", &Camera::transformWorldToScreen )
		.def( "transformWorldToCameraZ", &Camera::transformWorldToCameraZ )
		.def( "transformDistanceAcrossScreenToDistanceThruWorld", &Camera::py_transformDistanceAcrossScreenToDistanceThruWorld )
		.def( "transformDistanceAcrossScreenToDistanceThruWorld", &Camera::py_transformDistanceAcrossScreenToDistanceThruWorld2 )
		.def( "transformScreenToWorldScreenZ", &Camera::transformScreenToWorldScreenZ )
		.def( "transformScreenToWorldCameraZ", &Camera::transformScreenToWorldCameraZ )
		.def( "transformScreenAtTargetDistanceToWorld", &Camera::transformScreenAtTargetDistanceToWorld )
		.def( "getTopLeftCornerRay", &Camera::getTopLeftCornerRay, return_internal_reference<>() )
		.def( "getTopRightCornerRay", &Camera::getTopRightCornerRay, return_internal_reference<>() )
		.def( "getBottomLeftCornerRay", &Camera::getBottomLeftCornerRay, return_internal_reference<>() )
		.def( "getBottomRightCornerRay", &Camera::getBottomRightCornerRay, return_internal_reference<>() )
		.def( "computeRay", &Camera::computeRay )
		.def( "createBackfaceCull", &Camera::createBackfaceCull, return_value_policy<manage_new_object>() );
}



#endif
