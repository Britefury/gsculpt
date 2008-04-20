##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import Graphics

from Britefury.FileIO import IOXml


Graphics.Camera.cameraPosition = property( Graphics.Camera.getCameraPosition, Graphics.Camera.setCameraPosition )
Graphics.Camera.cameraProjection = property( Graphics.Camera.getCameraProjection, Graphics.Camera.setCameraProjection )
Graphics.Camera.projection = property( Graphics.Camera.getProjection )

Graphics.CameraPosition.position = property( Graphics.CameraPosition.getPosition )
Graphics.CameraPosition.targetPosition = property( Graphics.CameraPosition.getTargetPosition )
Graphics.CameraPosition.forwardAxis = property( Graphics.CameraPosition.getForwardAxis )
Graphics.CameraPosition.upAxis = property( Graphics.CameraPosition.getUpAxis )
Graphics.CameraPosition.rightAxis = property( Graphics.CameraPosition.getRightAxis )
Graphics.CameraPosition.worldToCameraMatrix = property( Graphics.CameraPosition.getWorldToCameraMatrix )
Graphics.CameraPosition.cameraToWorldMatrix = property( Graphics.CameraPosition.getCameraToWorldMatrix )



def __copymethod__(self):
	return self.clone()

def implementCopyMethods(cls):
	cls.__copy__ = __copymethod__


def registerXmlIOMethods(cls, className, readxml, writexml):
	cls.__readxml__ = readxml
	cls.__writexml__ = writexml
	IOXml.ioObjectFactoryRegister( className, cls )




def BackfaceCull__readxml__(self, xmlNode):
	pass

def BackfaceCull__writexml__(self, xmlNode):
	pass


registerXmlIOMethods( Graphics.BackfaceCull, 'BackfaceCull', BackfaceCull__readxml__, BackfaceCull__writexml__ )
implementCopyMethods( Graphics.BackfaceCull )



def OrthographicBackfaceCull__readxml__(self, xmlNode):
	super( Graphics.OrthographicBackfaceCull, self ).__readxml__( xmlNode )
	xmlNode.getChild( 'forward_vector' )  >>  self._forwardVector

def OrthographicBackfaceCull__writexml__(self, xmlNode):
	super( Graphics.OrthographicBackfaceCull, self ).__writexml__( xmlNode )
	xmlNode.addChild( 'forward_vector' )  <<  self._forwardVector


registerXmlIOMethods( Graphics.OrthographicBackfaceCull, 'OrthographicBackfaceCull', OrthographicBackfaceCull__readxml__, OrthographicBackfaceCull__writexml__ )



def PerspectiveBackfaceCull__readxml__(self, xmlNode):
	super( Graphics.PerspectiveBackfaceCull, self ).__readxml__( xmlNode )
	xmlNode.getChild( 'cam_position' )  >>  self._camPosition

def PerspectiveBackfaceCull__writexml__(self, xmlNode):
	super( Graphics.PerspectiveBackfaceCull, self ).__writexml__( xmlNode )
	xmlNode.addChild( 'cam_position' )  <<  self._camPosition


registerXmlIOMethods( Graphics.PerspectiveBackfaceCull, 'PerspectiveBackfaceCull', PerspectiveBackfaceCull__readxml__, PerspectiveBackfaceCull__writexml__ )






def Region2d__readxml__(self, xmlNode):
	pass

def Region2d__writexml__(self, xmlNode):
	pass


registerXmlIOMethods( Graphics.Region2d, 'Region2d', Region2d__readxml__, Region2d__writexml__ )
implementCopyMethods( Graphics.Region2d )




def RectangleRegion2d__readxml__(self, xmlNode):
	super( Graphics.RectangleRegion2d, self ).__readxml__( xmlNode )
	xmlNode.getChild( 'box' )  >>  self._box

def RectangleRegion2d__writexml__(self, xmlNode):
	super( Graphics.RectangleRegion2d, self ).__writexml__( xmlNode )
	xmlNode.addChild( 'box' )  <<  self._box


registerXmlIOMethods( Graphics.RectangleRegion2d, 'RectangleRegion2d', RectangleRegion2d__readxml__, RectangleRegion2d__writexml__ )




def CircleRegion2d__readxml__(self, xmlNode):
	super( Graphics.CircleRegion2d, self ).__readxml__( xmlNode )
	xmlNode.getChild( 'circle' )  >>  self._circle

def CircleRegion2d__writexml__(self, xmlNode):
	super( Graphics.CircleRegion2d, self ).__writexml__( xmlNode )
	xmlNode.addChild( 'circle' )  <<  self._circle


registerXmlIOMethods( Graphics.CircleRegion2d, 'CircleRegion2d', CircleRegion2d__readxml__, CircleRegion2d__writexml__ )




def PolylineRegion2d__readxml__(self, xmlNode):
	super( Graphics.PolylineRegion2d, self ).__readxml__( xmlNode )
	vertices = []
	verticesXml = xmlNode.getChild( 'vertices' )
	if verticesXml.isValid():
		for vertexXml in verticesXml.childrenNamed( 'vertex' ):
			v = Point2()
			v  <<  vertexXml
			vertices.append( v )
	self._vertices = vertices

def PolylineRegion2d__writexml__(self, xmlNode):
	super( Graphics.PolylineRegion2d, self ).__writexml__( xmlNode )
	verticesXml = xmlNode.addChild( 'vertices' )
	for v in self._vertices:
		verticesXml.addChild( 'vertex' )  <<  v


registerXmlIOMethods( Graphics.PolylineRegion2d, 'PolylineRegion2d', PolylineRegion2d__readxml__, PolylineRegion2d__writexml__ )




def LassoRegion2d__readxml__(self, xmlNode):
	super( Graphics.LassoRegion2d, self ).__readxml__( xmlNode )
	xmlNode.getChild( 'lasso' )  >>  self._lasso

def LassoRegion2d__writexml__(self, xmlNode):
	super( Graphics.LassoRegion2d, self ).__writexml__( xmlNode )
	xmlNode.addChild( 'lasso' )  <<  self._lasso


registerXmlIOMethods( Graphics.LassoRegion2d, 'LassoRegion2d', LassoRegion2d__readxml__, LassoRegion2d__writexml__ )





def Region3d__readxml__(self, xmlNode):
	pass

def Region3d__writexml__(self, xmlNode):
	pass


registerXmlIOMethods( Graphics.Region3d, 'Region3d', Region3d__readxml__, Region3d__writexml__ )
implementCopyMethods( Graphics.Region3d )




def ProjectedRegion3d__readxml__(self, xmlNode):
	super( Graphics.ProjectedRegion3d, self ).__readxml__( xmlNode )
	xmlNode.getChild( 'projection' )  >>  self._projection
	self._p_setRegion2d( xmlNode.getChild( 'region2d' ).readObject() )

def ProjectedRegion3d__writexml__(self, xmlNode):
	super( Graphics.ProjectedRegion3d, self ).__writexml__( xmlNode )
	xmlNode.addChild( 'projection' )  <<  self._projection
	xmlNode.addChild( 'region2d' ).writeObject( self._p_getRegion2d() )


registerXmlIOMethods( Graphics.ProjectedRegion3d, 'ProjectedRegion3d', ProjectedRegion3d__readxml__, ProjectedRegion3d__writexml__ )
