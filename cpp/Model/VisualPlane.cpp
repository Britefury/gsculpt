//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef VISUALPLANE_CPP__
#define VISUALPLANE_CPP__

#include <PlatformSpecific/IncludeGL.h>

#include <Graphics/Drawing.h>
#include <Graphics/GLMatrix.h>

#include <Math/Vector3.h>
#include <Math/Matrix4.h>

#include <Model/VisualPlane.h>



VisualPlane::VisualPlane() : position( Point3() ), rotation( Quaternion() ), size( 100.0 ), bVisible( true )
{
	refreshPlane();
}

VisualPlane::VisualPlane(const VisualPlane &vp)
					: position( vp.position ), rotation( vp.rotation ), size( vp.size ), bVisible( vp.bVisible )
{
	refreshPlane();
}

VisualPlane::VisualPlane(const Point3 &position, const Quaternion &rotation, double size, bool bVisible)
					: position( position ), rotation( rotation ), size( size ), bVisible( bVisible )
{
	refreshPlane();
}



const Point3 & VisualPlane::getPosition() const
{
	return position;
}

const Quaternion & VisualPlane::getRotation() const
{
	return rotation;
}

double VisualPlane::getSize() const
{
	return size;
}

const Plane & VisualPlane::getPlane() const
{
	return plane;
}

bool VisualPlane::isVisible() const
{
	return bVisible;
}



void VisualPlane::setPosition(const Point3 &pos)
{
	position = pos;
	refreshPlane();
}

void VisualPlane::setRotation(const Quaternion &rot)
{
	rotation = rot;
	refreshPlane();
}

void VisualPlane::setSize(double sz)
{
	size = sz;
	refreshPlane();
}

void VisualPlane::setVisible(bool bVisible)
{
	this->bVisible = bVisible;
}



void VisualPlane::draw()
{
	if ( bVisible )
	{
		Matrix4 m = Matrix4::scale( size )  *  Matrix4::rotate( rotation )  *  Matrix4::translate( position.toVector3() );

		glPushMatrix();
		glMultMatrix( m );

		glBegin( GL_QUADS );
		glVertex3d( -0.5, -0.5, 0.0  );
		glVertex3d( 0.5, -0.5, 0.0  );
		glVertex3d( 0.5, 0.5, 0.0  );
		glVertex3d( -0.5, 0.5, 0.0  );
		glEnd();

		glLineWidth( 4.0 );
		glBegin( GL_LINE_LOOP );
		glVertex3d( -0.5, -0.5, 0.0  );
		glVertex3d( 0.5, -0.5, 0.0  );
		glVertex3d( 0.5, 0.5, 0.0  );
		glVertex3d( -0.5, 0.5, 0.0  );
		glEnd();
		glLineWidth( 1.0 );

		glPopMatrix();
	}
}

void VisualPlane::paint(const Colour3f &colour, float alpha, PaintLayer paintLayer)
{
	if ( paintLayer == PAINTLAYER_OVERLAY_BLEND )
	{
		glDepthMask( false );
		glColor4( colour, alpha );
		draw();
		glDepthMask( true );
	}
	else if ( paintLayer == PAINTLAYER_OVERLAY_BLEND_NODEPTH )
	{
		glDepthMask( false );
		glColor3( colour );
		draw();
		glDepthMask( true );
	}
}


void VisualPlane::refreshPlane()
{
	Vector3 normal = Vector3( 0.0, 0.0, 1.0 )  *  Matrix4::rotate( rotation );
	plane = Plane( normal, position );
}


#endif
