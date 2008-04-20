##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.WorkArea import Viewport3d
from Britefury.WorkArea.Viewport3d import *

from RequestPainter import RequestPainter




class RequestViewportPainter (object):
	class Painter3d (Viewport3dPainter):
		def __init__(self, painter):
			self._painter = painter

		def evPaint3d(self, viewport, paintLayer):
			self._painter._painter.paintObject3d( viewport.viewportSettings, paintLayer, self._painter._bBackground )




	def __init__(self, painterFactory, bBackground):
		assert painterFactory is not None
		self._painter = RequestPainter( painterFactory )
		self._bBackground = bBackground
		self._painter3d = RequestViewportPainter.Painter3d( self )


	def addToViewport(self, viewport):
		if isinstance( viewport, Viewport3d ):
			viewport.addPainter( self._painter3d )

	def removeFromViewport(self, viewport):
		if isinstance( viewport, Viewport3d ):
			viewport.removePainter( self._painter3d )


	def addToView(self, view):
		view.addPainter( self._painter3d, Viewport3d )

	def removeFromView(self, view):
		view.removePainter( self._painter3d, Viewport3d )



	def _p_setObjectSource(self, source):
		self._painter.painterObjectSource = source

	painterObjectSource = property( None, _p_setObjectSource )

