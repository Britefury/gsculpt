##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.FormLayout import FormLayout

from Britefury.Tool.Tool import Tool

from Britefury.WorkArea.Viewport import ViewportEventHandler
from Britefury.WorkArea.Viewport3d import Viewport3d

from Britefury.Model.Model import GSProductModel





class FocusTool (Tool, ViewportEventHandler):
	def __init__(self, nodeEditor, view, parentWindow):
		super( FocusTool, self ).__init__( view, parentWindow )
		self._nodeEditor = nodeEditor
		self._view = view
		self.setTitleWidgetText( _( 'Focus' ) )
		self._pageTitle = _( 'Focus' )

		layout = FormLayout()
		layout.row()
		layout.row()  <<  _( 'Click on a point on the current' )
		layout.row()  <<  _( 'model to focus on that point.' )
		layout.row()
		layout.finalise()
		layout.getWidget().show()
		self._o_packContents( layout.getWidget() )




	def _o_installEventHandlers(self):
		self._view.addEventHandler( self, Viewport3d )


	def _o_uninstallEventHandlers(self):
		self._view.removeEventHandler( self, Viewport3d )




	def evPrimaryButtonDown(self, viewport):
		ray = viewport.computeRay()
		product = self._nodeEditor.displayedProduct
		if product is not None:
			if isinstance( product, GSProductModel ):
				bSuccess, t, intersection = product.raytrace( ray, False )				# HACK: get proper value for backface culling
				if bSuccess:
					viewport.focusOn( intersection )
					self._o_done()


