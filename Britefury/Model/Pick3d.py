##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Point2, Projection

from Britefury.FileIO.IOXml import ioXmlReadBoolNode, ioXmlWriteBoolNode, ioObjectFactoryRegister

from Britefury.Model.Pick import Pick

from Britefury.Model.Model import MPick



class Pick3d (Pick):
	__version__ = 1

	def __init__(self, screenPos=None, projection=None, bBackfaceCulling=False, bSolidPick=True):
		if screenPos is None:
			screenPos = Point2()
		if projection is None:
			projection = Projection()
		self.screenPos = screenPos
		self.projection = projection
		self.bBackfaceCulling = bBackfaceCulling
		self.bSolidPick = bSolidPick
		self.bAllowPickOutsideBounds = True


	def computeRay(self):
		return self.projection.computeRay( self.screenPos )


	def toMPick(self):
		return MPick( self.screenPos, self.projection, self.bBackfaceCulling, self.bAllowPickOutsideBounds, self.bSolidPick )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'screen_pos' )  >>  self.screenPos
		xmlNode.getChild( 'projection' )  >>  self.projection
		self.bBackfaceCulling = ioXmlReadBoolNode( xmlNode.getChild( 'b_backface_culling' ), self.bBackfaceCulling )
		self.bSolidPick = ioXmlReadBoolNode( xmlNode.getChild( 'b_solid_pick' ), self.bSolidPick )
		if xmlNode.getVersionNumberProperty() < 1:
			self.bAllowPickOutsideBounds = False

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'screen_pos' )  <<  self.screenPos
		xmlNode.addChild( 'projection' )  <<  self.projection
		ioXmlWriteBoolNode( xmlNode.addChild( 'b_backface_culling' ), self.bBackfaceCulling )
		ioXmlWriteBoolNode( xmlNode.addChild( 'b_solid_pick' ), self.bSolidPick )
		xmlNode.setVersionNumberProperty( self.__version__ )


ioObjectFactoryRegister( 'Pick3d', Pick3d )


