##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO import IOXml

from Britefury.ProceduralCore.GSScene import GSScene
from Britefury.View.DrawingPlane import DrawingPlane

import GSProjectCommandTracker



class GSProject (object):
	trackerClass = GSProjectCommandTracker.GSProjectCommandTracker

	def __init__(self):
		self._commandTracker_ = None
		self._scene = GSScene()
		self._drawingPlane = DrawingPlane()


	def _p_getScene(self):
		return self._scene

	def _p_getDrawingPlane(self):
		return self._drawingPlane


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'scene' )  >>  self._scene
		xmlNode.getChild( 'drawing_plane' )  >>  self._drawingPlane


	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'scene' )  <<  self._scene
		xmlNode.addChild( 'drawing_plane' )  <<  self._drawingPlane


	scene = property( _p_getScene, doc='The scene' )
	drawingPlane = property( _p_getDrawingPlane, doc='The drawing plane' )

	__doc__ = _( """gSculpt Project
	GSProject() -> a gSculpt project with an empty scene

	- XML writeable""" )


IOXml.ioObjectFactoryRegister( 'GSProject', GSProject )

