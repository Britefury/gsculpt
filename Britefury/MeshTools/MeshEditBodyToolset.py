##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Tool.Toolset import Toolset

from Britefury.MeshTools.MeshEditBodyToolGroup import MeshEditBodyToolGroup
from Britefury.ModelTools.ModelVirtualMirrorToolGroup import ModelVirtualMirrorToolGroup
from Britefury.TransformationTools.TransformationToolGroup import TransformationToolGroup

from Britefury.TransformationTools.ToolTransformationBase import TransformationTarget


class MeshEditBodyToolset (Toolset):
	def __init__(self, nodeEditor, sceneEditorTools, backgroundLayerPainter, view, parentWindow, toolPanel, commandHistory):
		super( MeshEditBodyToolset, self ).__init__( _( 'Body' ) )
		meshBodyGroup = MeshEditBodyToolGroup( nodeEditor, sceneEditorTools, backgroundLayerPainter, view, parentWindow, toolPanel, commandHistory )
		self._virtualMirrorGroup = ModelVirtualMirrorToolGroup( nodeEditor )
		xformGroup = TransformationToolGroup( nodeEditor, TransformationTarget.BODY, True, True )
		self.addToolGroup( meshBodyGroup )
		self.addToolGroup( self._virtualMirrorGroup )
		self.addToolGroup( xformGroup )



	def attachProductCell(self, cell):
		self._virtualMirrorGroup.attachModelCell( cell )

	def detachProductCell(self):
		self._virtualMirrorGroup.detachModelCell()


