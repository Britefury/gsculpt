##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.FormLayout import FormLayout

from Britefury.Tool.Tool import Tool

from Britefury.WorkArea.Viewport import ViewportEventHandler, ViewportContextButtonListener
from Britefury.WorkArea.Viewport3d import Viewport3d

from Britefury.Model.Model import GSProductModel





class SelectNodeTool (Tool, ViewportEventHandler, ViewportContextButtonListener):
	def __init__(self, sceneEditorTools, backgroundLayerPainter, view, parentWindow):
		super( SelectNodeTool, self ).__init__( view, parentWindow )
		self._sceneEditorTools = sceneEditorTools
		self._backgroundLayerPainter = backgroundLayerPainter
		self._view = view
		self.setTitleWidgetText( _( 'Choose Model' ) )
		self._pageTitle = _( 'ChMdl' )

		layout = FormLayout()
		layout.row()
		layout.textBlock( _( 'Click on a model in a viewport to choose it.' ) )
		layout.row()
		layout.finalise()
		layout.getWidget().show()
		self._o_packContents( layout.getWidget() )



	def _o_installEventHandlers(self):
		self._view.addEventHandler( self, Viewport3d )
		self._view.addContextButtonListener( self, Viewport3d )


	def _o_uninstallEventHandlers(self):
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removeContextButtonListener( self, Viewport3d )




	def evPrimaryButtonDown(self, viewport):
		ray = viewport.computeRay()

		node = None
		t = None
		intersection = None

		node, t, intersection, _clippedRay = self._backgroundLayerPainter.chooseNode( ray, False )		# HACK: get proper value for backface culling
		if node is not None:
			ray = _clippedRay

		product = self._sceneEditorTools.nodeEditor.displayedProduct
		if product is not None:
			if isinstance( product, GSProductModel ):
				_bSuccess, _t, _intersection, _clippedRay = product.raytraceClipRay( ray, False )		# HACK: get proper value for backface culling
				if _bSuccess:
					node = self._sceneEditorTools.currentNode
					t = _t
					intersection = _intersection
					ray = _clippedRay



		self._sceneEditorTools.currentNode = node


	def evContextButton(self, viewport):
		self._o_done()


