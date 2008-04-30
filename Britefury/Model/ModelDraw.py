##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.GL.GL import *

from Britefury.Math.Math import Colour3f

from Britefury.View.View import PaintLayer

from Britefury.Graphics.Graphics import *



VERTEX_PICKHIGHLIGHT_POINT_SIZE						= 8.0
EDGE_PICKHIGHLIGHT_LINE_WIDTH						= 5.0

OVERLAY_FACTOR									= 0.35

VERTEX_RETICLE_COLOUR								= Colour3f( 1.0, 0.0, 1.0 )

TARGET_HIGHLIGHT_COLOUR							= Colour3f( 0.0, 0.7, 0.0 )
MARKED_TARGET_HIGHLIGHT_COLOUR					= Colour3f( 0.7, 0.7, 0.0 )
INVALID_TARGET_HIGHLIGHT_COLOUR					= Colour3f( 0.65, 0.0, 0.0 )
WARNING_TARGET_HIGHLIGHT_COLOUR					= Colour3f( 0.7, 0.35, 0.0 )
PATH_HIGHLIGHT_COLOUR								= Colour3f( 0.25, 0.1, 1.0 )

TWEAK_CONSTRAINT_WIDGET_ALPHA						= 0.6
TWEAK_CONSTRAINT_WIDGET_SIZE						= 0.0375
TWEAK_CONSTRAINT_WIDGET_BRIGHTNESS				= 0.5

MODIFICATION1_HIGHLIGHT_COLOUR						= Colour3f( 0.5, 0.0, 1.0 )
MODIFICATION2_HIGHLIGHT_COLOUR						= Colour3f( 0.0, 0.4, 0.8 )
MODIFICATION_HIGHLIGHT_ALPHA						= 0.55

DRAWING_PLANE_COLOUR								= Colour3f( 0.15, 0.0, 0.15 )
DRAWING_PLANE_ALPHA								= 0.7




def computeOverlayColour(colour):
	return colour * OVERLAY_FACTOR


def drawVertexPickHighlight(position, colour, paintLayer):
	if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY  or  paintLayer == PaintLayer.PAINTLAYER_WIREFRAME:
		if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
			colour = computeOverlayColour( colour );

		glColor3Colour3f( colour )
		glPointSize( VERTEX_PICKHIGHLIGHT_POINT_SIZE )
		glBegin( GL_POINTS )
		glVertex3Point3( position )
		glEnd()
		glPointSize( 1.0 )


def drawEdgePickHighlight(segment, colour, paintLayer):
	if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY  or  paintLayer == PaintLayer.PAINTLAYER_WIREFRAME:
		if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
			colour = computeOverlayColour( colour );

		glColor3Colour3f( colour )
		glLineWidth( EDGE_PICKHIGHLIGHT_LINE_WIDTH )
		glBegin( GL_LINES )
		glVertex3Point3( segment.a )
		glVertex3Point3( segment.b )
		glEnd()
		glLineWidth( 1.0 )


def drawFacePickHighlight(tris, colour, paintLayer):
	if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY  or  paintLayer == PaintLayer.PAINTLAYER_WIREFRAME:
		if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
			colour = computeOverlayColour( colour );

		glColor3Colour3f( colour )
		glBegin( GL_TRIANGLES )
		for tri in tris:
			glVertex3Point3( tri.a )
			glVertex3Point3( tri.b )
			glVertex3Point3( tri.c )
		glEnd()



def drawPickPath(pathPoints, colour, paintLayer):
	if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY  or  paintLayer == PaintLayer.PAINTLAYER_WIREFRAME:
		if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
			colour = computeOverlayColour( colour );

		glColor3Colour3f( colour )
		glBegin( GL_LINE_STRIP )
		for point in pathPoints:
			glVertex3Point3( point )
		glEnd()



#	f0	78												1111.....1111
#	80	08												1...........1
#	80	08												1...........1
#	80	08												1...........1
#	00	00												.............
#	00	00												.............
#	00	00												.............
#	00	00												.............
#	00	00												.............
#	80	08												1...........1
#	80	08												1...........1
#	80	08												1...........1
#	f0	78												1111.....1111
reticleBitmap = '\xf0\x78'  '\x80\x08'  '\x80\x08'  '\x80\x08'  '\x00\x00'  '\x00\x00'  '\x00\x00'  '\x00\x00'  '\x00\x00'  '\x80\x08'  '\x80\x08'  '\x80\x08'  '\xf0\x78'
def drawVertexReticle(position, bTransparencyLayer):
	colour = VERTEX_RETICLE_COLOUR

	if bTransparencyLayer:
		colour = computeOverlayColour( colour )

	glColor3Colour3f( VERTEX_RETICLE_COLOUR )
	glRasterPos3Point3( position )
	glBitmap( 13, 13, 6, 6, 0, 0, reticleBitmap )





#	04	00												......|......
#	04	00												......|......
#	04	00												......|......
#	04	00												......|......
#	04	00												......|......
#	04	00												......|......
#	fd	f8'												||||||.||||||
#	04	00												......|......
#	04	00												......|......
#	04	00												......|......
#	04	00												......|......
#	04	00												......|......
#	04	00												......|......
splitBitmap = '\x04\x00'  	'\x04\x00'  '\x04\x00'  '\x04\x00'  '\x04\x00'  '\x04\x00'  '\xfd\xf8'  '\x04\x00'  '\x04\x00'  '\x04\x00'  '\x04\x00'  '\x04\x00'  '\x04\x00'
def drawSplitPosition(position):
	glColor3Colour3f( VERTEX_RETICLE_COLOUR )
	glRasterPos3Point3( position )
	glBitmap( 13, 13, 6, 6, 0, 0, splitBitmap )



#	38													..|||..
#	44													.|...|.
#	82													|.....|
#	82													|.....|
#	82													|.....|
#	44													.|...|.
#	38													..|||..
circleBitmap = '\x38'  '\x44'  '\x82'  '\x82'  '\x82'  '\x44'  '\x38'
def drawCircleHighlight(position, colour, paintLayer):
	if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY  or  paintLayer == PaintLayer.PAINTLAYER_WIREFRAME:
		if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
			colour = computeOverlayColour( colour )

		glColor3Colour3f( colour )
		glRasterPos3Point3( position )
		glBitmap( 7, 7, 4, 4, 0, 0, circleBitmap )





def drawSegment(segment, colour, paintLayer):
	if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY  or  paintLayer == PaintLayer.PAINTLAYER_WIREFRAME:
		if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
			colour = computeOverlayColour( colour )

		glColor4Colour3fA( colour, MODIFICATION_HIGHLIGHT_ALPHA )
		glBegin( GL_LINES )
		glVertex3Point3( segment.a )
		glVertex3Point3( segment.b )
		glEnd()



def drawSegments(segments, colour, paintLayer):
	if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY  or  paintLayer == PaintLayer.PAINTLAYER_WIREFRAME:
		if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
			colour = computeOverlayColour( colour )

		glColor4Colour3fA( colour, MODIFICATION_HIGHLIGHT_ALPHA )
		glBegin( GL_LINES )
		for segment in segments:
			glVertex3Point3( segment.a )
			glVertex3Point3( segment.b )
		glEnd()



def drawLineStrip(points, colour, paintLayer):
	if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY  or  paintLayer == PaintLayer.PAINTLAYER_WIREFRAME:
		if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
			colour = computeOverlayColour( colour )

		glColor4Colour3fA( colour, MODIFICATION_HIGHLIGHT_ALPHA )
		glBegin( GL_LINE_STRIP )
		for point in points:
			glVertex3Point3( point )
		glEnd()




def drawMarkBox(box):
	glColor4d( 0.0, 0.5, 1.0, 0.5 )

	lower = box.getLower()
	upper = box.getUpper()

	glBegin( GL_LINE_LOOP )
	glVertex2d( lower.x, lower.y )
	glVertex2d( upper.x, lower.y )
	glVertex2d( upper.x, upper.y )
	glVertex2d( lower.x, upper.y )
	glEnd()


def drawMarkCircle(circle):
	glColor4d( 0.0, 0.5, 1.0, 0.5 )
	glDrawCircle2( circle.getCentre(), circle.getRadius() )

#void modelDrawMarkPolyline(const Array<Point2> &vertices)
#{
	#glColor4d( 0.0, 0.5, 1.0, 0.5 );

	#glBegin( GL_LINE_STRIP );
	#for (int i = 0; i < vertices.size(); i++)
	#{
		#glVertex2d( vertices[i].x, vertices[i].y );
	#}
	#glEnd();
#}

#void modelDrawMarkLasso(const Array<Point2> &vertices, bool closed)
#{
	#glColor4d( 0.0, 0.5, 1.0, 0.5 );

	#if ( closed )
	#{
		#glBegin( GL_LINE_LOOP );
	#}
	#else
	#{
		#glBegin( GL_LINE_STRIP );
	#}

	#for (int i = 0; i < vertices.size(); i++)
	#{
		#glVertex2d( vertices[i].x, vertices[i].y );
	#}
	#glEnd();
#}
