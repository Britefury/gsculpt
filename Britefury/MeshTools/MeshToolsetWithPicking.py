##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Tool.Toolset import Toolset, TOOLSET_EVENTHANDLER_PRIORITY

from Britefury.WorkArea.Viewport3d import Viewport3d

from Britefury.Mesh.MeshPickHelper import MeshPickHelper



class MeshToolsetWithPicking (Toolset):
	class MeshPick (MeshPickHelper):
		def __init__(self, toolset):
			super( MeshToolsetWithPicking.MeshPick, self ).__init__()
			self._vpType = None
			self._toolset = toolset


		def attachView(self, view, vpType):
			MeshPickHelper.attachView( self, view )
			self._vpType = vpType
			view.addEventHandler( self, vpType, TOOLSET_EVENTHANDLER_PRIORITY )
			view.addPainter( self, vpType )
			view.postRedraw()

		def detachView(self):
			self._meshPickHelperView.removeEventHandler( self, self._vpType )
			self._meshPickHelperView.removePainter( self, self._vpType )
			self._meshPickHelperView.postRedraw()
			MeshPickHelper.detachView( self )
			self._vpType = None


		def _p_pickConfirmed(self, viewport, pick):
			self._toolset._p_onPick( pick )



	def __init__(self, title, nodeEditor):
		super( MeshToolsetWithPicking, self ).__init__( title )
		self._nodeEditor = nodeEditor
		self._meshPick = self._p_createMeshPick()


	def attachProductCell(self, cell):
		self._meshPick.attachMeshCell( cell )

	def detachProductCell(self):
		self._meshPick.detachMeshCell()


	def installEventHandlers(self, view):
		super( MeshToolsetWithPicking, self ).installEventHandlers( view )
		self._meshPick.attachView( view, Viewport3d )

	def uninstallEventHandlers(self):
		super( MeshToolsetWithPicking, self ).uninstallEventHandlers()
		self._meshPick.detachView()


	def _p_createMeshPick(self):
		assert False, 'abstract'


	def _p_onPick(self, pick):
		currentFunction = self._nodeEditor.currentFunction

		if currentFunction is not None:
			if isinstance( currentFunction, self._p_getPickProcedureType() ):
				self._nodeEditor.commandHistory.freeze()
				self._nodeEditor.editCurrentFunction()
				currentFunction.cells.targetList.literalValue = currentFunction.cells.targetList.value + [ pick ]
				self._nodeEditor.commandHistory.finishCommand()
				self._nodeEditor.commandHistory.thaw()
				return

		self._nodeEditor.commandHistory.freeze()
		proc = self._p_getPickProcedureType()()
		self._nodeEditor.addProcedureSelectAndEdit( proc )
		proc.cells.targetList.literalValue = proc.cells.targetList.value + [ pick ]
		self._nodeEditor.commandHistory.finishCommand()
		self._nodeEditor.commandHistory.thaw()


	def _p_getPickProcedureType(self):
		assert False, 'abstract'
