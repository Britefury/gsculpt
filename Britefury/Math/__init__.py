##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import Math

from Britefury.FileIO import IOXml

from Britefury.Math import MathCellTypes


def lerp(a, b, t):
	return a  +  ( b - a ) * t


#
# Properties of Projection
#

Math.Projection.worldToScreenMatrix = property( Math.Projection.getWorldToScreenMatrix, Math.Projection.setWorldToScreenMatrix )
Math.Projection.screenToWorldMatrix = property( Math.Projection.getScreenToWorldMatrix, Math.Projection.setScreenToWorldMatrix )
Math.Projection.nearClipPlane = property( Math.Projection.getNearClipPlane, Math.Projection.setNearClipPlane )
Math.Projection.farClipPlane = property( Math.Projection.getFarClipPlane, Math.Projection.setFarClipPlane )



#
# COPYING
#

def __primitive__copy__(self):
	return self.__class__( self )

def __primitive__deepcopy__(self, memo):
	return self.__class__( self )


def implementPrimitiveCopyMethods(cls):
	cls.__copy__ = __primitive__copy__
	cls.__deepcopy__ = __primitive__deepcopy__



implementPrimitiveCopyMethods( Math.Axes2 )
implementPrimitiveCopyMethods( Math.Axes3 )
implementPrimitiveCopyMethods( Math.BBox2 )
implementPrimitiveCopyMethods( Math.BBox3 )
implementPrimitiveCopyMethods( Math.Circle2 )
implementPrimitiveCopyMethods( Math.Colour3f )
implementPrimitiveCopyMethods( Math.EulerRotation )
implementPrimitiveCopyMethods( Math.Frustum )
implementPrimitiveCopyMethods( Math.PerspectiveFrustum )
implementPrimitiveCopyMethods( Math.OrthographicFrustum )
implementPrimitiveCopyMethods( Math.Line3 )
implementPrimitiveCopyMethods( Math.Matrix3 )
implementPrimitiveCopyMethods( Math.Matrix4 )
implementPrimitiveCopyMethods( Math.Plane )
implementPrimitiveCopyMethods( Math.Point2 )
implementPrimitiveCopyMethods( Math.Point2f )
implementPrimitiveCopyMethods( Math.Point2i )
implementPrimitiveCopyMethods( Math.Point3 )
implementPrimitiveCopyMethods( Math.Point3f )
implementPrimitiveCopyMethods( Math.Point4 )
implementPrimitiveCopyMethods( Math.Polygon2 )
implementPrimitiveCopyMethods( Math.Polygon3 )
implementPrimitiveCopyMethods( Math.Projection )
implementPrimitiveCopyMethods( Math.Quaternion )
implementPrimitiveCopyMethods( Math.Ray3 )
implementPrimitiveCopyMethods( Math.Segment2 )
implementPrimitiveCopyMethods( Math.Segment3 )
implementPrimitiveCopyMethods( Math.Triangle2 )
implementPrimitiveCopyMethods( Math.Triangle3 )
implementPrimitiveCopyMethods( Math.Vector2 )
implementPrimitiveCopyMethods( Math.Vector2f )
implementPrimitiveCopyMethods( Math.Vector2i )
implementPrimitiveCopyMethods( Math.Vector3 )
implementPrimitiveCopyMethods( Math.Vector3f )




# POLYGON ITERATORS
def Polygon__iter__(self):
	for i in xrange( 0, len( self ) ):
		yield self[i]

Math.Polygon2.__iter__ = Polygon__iter__
Math.Polygon3.__iter__ = Polygon__iter__




#
# XML IO
#

def implementXml(typeName, cls, readFunction, writeFunction):
	cls.__readxml__ = readFunction
	cls.__writexml__ = writeFunction
	IOXml.ioObjectFactoryRegister( typeName, cls )



def xyFloat__readxml__(self, xmlNode):
	self.x = IOXml.ioXmlReadFloatProp( xmlNode.property( 'x' ), self.x )
	self.y = IOXml.ioXmlReadFloatProp( xmlNode.property( 'y' ), self.y )

def xyFloat__writexml__(self, xmlNode):
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'x' ), self.x )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'y' ), self.y )


def xyzFloat__readxml__(self, xmlNode):
	self.x = IOXml.ioXmlReadFloatProp( xmlNode.property( 'x' ), self.x )
	self.y = IOXml.ioXmlReadFloatProp( xmlNode.property( 'y' ), self.y )
	self.z = IOXml.ioXmlReadFloatProp( xmlNode.property( 'z' ), self.z )

def xyzFloat__writexml__(self, xmlNode):
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'x' ), self.x )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'y' ), self.y )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'z' ), self.z )


def xyzwFloat__readxml__(self, xmlNode):
	self.x = IOXml.ioXmlReadFloatProp( xmlNode.property( 'x' ), self.x )
	self.y = IOXml.ioXmlReadFloatProp( xmlNode.property( 'y' ), self.y )
	self.z = IOXml.ioXmlReadFloatProp( xmlNode.property( 'z' ), self.z )
	self.w = IOXml.ioXmlReadFloatProp( xmlNode.property( 'w' ), self.w )

def xyzwFloat__writexml__(self, xmlNode):
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'x' ), self.x )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'y' ), self.y )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'z' ), self.z )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'w' ), self.w )


def rgbFloat__readxml__(self, xmlNode):
	self.r = IOXml.ioXmlReadFloatProp( xmlNode.property( 'r' ), self.r )
	self.g = IOXml.ioXmlReadFloatProp( xmlNode.property( 'g' ), self.g )
	self.b = IOXml.ioXmlReadFloatProp( xmlNode.property( 'b' ), self.b )

def rgbFloat__writexml__(self, xmlNode):
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'r' ), self.r )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'g' ), self.g )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'b' ), self.b )


def Matrix4__readxml__(self, xmlNode):
	self[0,0] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm00' ), self[0,0] )
	self[0,1] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm01' ), self[0,1] )
	self[0,2] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm02' ), self[0,2] )
	self[0,3] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm03' ), self[0,3] )
	self[1,0] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm10' ), self[1,0] )
	self[1,1] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm11' ), self[1,1] )
	self[1,2] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm12' ), self[1,2] )
	self[1,3] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm13' ), self[1,3] )
	self[2,0] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm20' ), self[2,0] )
	self[2,1] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm21' ), self[2,1] )
	self[2,2] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm22' ), self[2,2] )
	self[2,3] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm23' ), self[2,3] )
	self[3,0] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm30' ), self[3,0] )
	self[3,1] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm31' ), self[3,1] )
	self[3,2] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm32' ), self[3,2] )
	self[3,3] = IOXml.ioXmlReadFloatProp( xmlNode.property( 'm33' ), self[3,3] )

def Matrix4__writexml__(self, xmlNode):
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm00' ), self[0,0] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm01' ), self[0,1] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm02' ), self[0,2] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm03' ), self[0,3] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm10' ), self[1,0] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm11' ), self[1,1] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm12' ), self[1,2] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm13' ), self[1,3] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm20' ), self[2,0] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm21' ), self[2,1] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm22' ), self[2,2] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm23' ), self[2,3] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm30' ), self[3,0] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm31' ), self[3,1] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm32' ), self[3,2] )
	IOXml.ioXmlWriteFloatProp( xmlNode.property( 'm33' ), self[3,3] )


def Plane__readxml__(self, xmlNode):
	xmlNode.getChild( 'n' )  >>  self.n
	self.d = IOXml.ioXmlReadFloatNode( xmlNode.getChild( 'd' ), self.d )

def Plane__writexml__(self, xmlNode):
	xmlNode.addChild( 'n' )  <<  self.n
	IOXml.ioXmlWriteFloatNode( xmlNode.addChild( 'd' ), self.d )


def Projection__readxml__(self, xmlNode):
	xmlNode.getChild( 'world_to_screen_matrix' )  >>  self.worldToScreenMatrix
	self.screenToWorldMatrix = self.worldToScreenMatrix.inverse()
	xmlNode.getChild( 'near_clip_plane' )  >>  self.nearClipPlane
	xmlNode.getChild( 'far_clip_plane' )  >>  self.farClipPlane

def Projection__writexml__(self, xmlNode):
	xmlNode.addChild( 'world_to_screen_matrix' )  <<  self.worldToScreenMatrix
	xmlNode.addChild( 'near_clip_plane' )  <<  self.nearClipPlane
	xmlNode.addChild( 'far_clip_plane' )  <<  self.farClipPlane


def Segment__readxml__(self, xmlNode):
	xmlNode.getChild( 'a' )  >>  self.a
	xmlNode.getChild( 'b' )  >>  self.b

def Segment__writexml__(self, xmlNode):
	xmlNode.addChild( 'a' )  <<  self.a
	xmlNode.addChild( 'b' )  <<  self.b


def BBox__readxml__(self, xmlNode):
	xmlNode.getChild( 'lower' )  >>  self._lower
	xmlNode.getChild( 'upper' )  >>  self._upper

def BBox__writexml__(self, xmlNode):
	xmlNode.addChild( 'lower' )  <<  self._lower
	xmlNode.addChild( 'upper' )  <<  self._upper


def Polygon2__readxml__(self, xmlNode):
	self.clear()
	verticesXml = xmlNode.getChild( 'vertices' )
	if verticesXml.isValid():
		for vertexXml in verticesXml.childrenNamed( 'vertex' ):
			v = Point2()
			vertexXml  >>  v
			self.append( v )

def Polygon2__writexml__(self, xmlNode):
	verticesXml = xmlNode.addChild( 'vertices' )
	for v in self:
		verticesXml.addChild( 'vertex' )  <<  v


def Polygon3__readxml__(self, xmlNode):
	self.clear()
	verticesXml = xmlNode.getChild( 'vertices' )
	if verticesXml.isValid():
		for vertexXml in verticesXml.childrenNamed( 'vertex' ):
			v = Point3()
			vertexXml  >>  v
			self.append( v )

def Polygon3__writexml__(self, xmlNode):
	verticesXml = xmlNode.addChild( 'vertices' )
	for v in self:
		verticesXml.addChild( 'vertex' )  <<  v




implementXml( 'Point2', Math.Point2, xyFloat__readxml__, xyFloat__writexml__ )
implementXml( 'Point2f', Math.Point2f, xyFloat__readxml__, xyFloat__writexml__ )
implementXml( 'Vector2', Math.Vector2, xyFloat__readxml__, xyFloat__writexml__ )
implementXml( 'Vector2f', Math.Vector2f, xyFloat__readxml__, xyFloat__writexml__ )

implementXml( 'Point3', Math.Point3, xyzFloat__readxml__, xyzFloat__writexml__ )
implementXml( 'Point3f', Math.Point3f, xyzFloat__readxml__, xyzFloat__writexml__ )
implementXml( 'Vector3', Math.Vector3, xyzFloat__readxml__, xyzFloat__writexml__ )
implementXml( 'Vector3f', Math.Vector3f, xyzFloat__readxml__, xyzFloat__writexml__ )

implementXml( 'Colour3f', Math.Colour3f, rgbFloat__readxml__, rgbFloat__writexml__ )

implementXml( 'Point4', Math.Point4, xyzwFloat__readxml__, xyzwFloat__writexml__ )
implementXml( 'Quaternion', Math.Quaternion, xyzwFloat__readxml__, xyzwFloat__writexml__ )

implementXml( 'Matrix4', Math.Matrix4, Matrix4__readxml__, Matrix4__writexml__ )

implementXml( 'Plane', Math.Plane, Plane__readxml__, Plane__writexml__ )

implementXml( 'Projection', Math.Projection, Projection__readxml__, Projection__writexml__ )

implementXml( 'Segment2', Math.Segment2, Segment__readxml__, Segment__writexml__ )
implementXml( 'Segment3', Math.Segment3, Segment__readxml__, Segment__writexml__ )

implementXml( 'BBox2', Math.BBox2, BBox__readxml__, BBox__writexml__ )
implementXml( 'BBox3', Math.BBox3, BBox__readxml__, BBox__writexml__ )

implementXml( 'Polygon2', Math.Polygon2, Polygon2__readxml__, Polygon2__writexml__ )
implementXml( 'Polygon3', Math.Polygon3, Polygon3__readxml__, Polygon3__writexml__ )




def make_xy__str__(typeName):
	def xy__str__(self):
		return '%s(%f,%f)' % ( typeName, self.x, self.y )
	return xy__str__

def make_xyz__str__(typeName):
	def xyz__str__(self):
		return '%s(%f,%f,%f)' % ( typeName, self.x, self.y, self.z )
	return xyz__str__

Math.Vector2.__str__ = make_xy__str__( 'Vector2' )
Math.Vector3.__str__ = make_xyz__str__( 'Vector3' )
Math.Point2.__str__ = make_xy__str__( 'Point2' )
Math.Point3.__str__ = make_xyz__str__( 'Point3' )



def _BBox3__str__(self):
	return 'BBox3(%s -> %s)'  %  ( self.getLower(), self.getUpper() )

Math.BBox3.__str__ = _BBox3__str__