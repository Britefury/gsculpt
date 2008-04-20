##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.WorkArea.Viewport import ViewportEventHandler

from Britefury.Mesh import GSProductMesh


class MeshManipulatorHelper (ViewportEventHandler):
	def __init__(self):
		super( MeshManipulatorHelper, self ).__init__()
		self._meshManipHelperView = None
		self._meshCell = None
		self._bSensitive = True


	def attachView(self, view):
		assert self._meshManipHelperView is None, 'view already attached'
		self._meshManipHelperView = view

	def detachView(self):
		assert self._meshManipHelperView is not None, 'no view attached'
		self._meshManipHelperView = None


	def attachMeshCell(self, meshCell):
		assert self._meshCell is None, 'mesh source already attached'
		self._meshCell = meshCell
		self._meshCell.changedSignal.connect( self._p_onMeshCellChanged )

	def detachMeshCell(self):
		assert self._meshCell is not None, 'no mesh source attached'
		self._meshCell.changedSignal.disconnect( self._p_onMeshCellChanged )
		self._meshCell = None


	def _o_setMeshManipulatorHelperSensitivity(self, bSensitive):
		bSensitivityChanged = bSensitive != self._bSensitive
		self._bSensitive = bSensitive
		if self._meshManipHelperView is not None  and  bSensitivityChanged:
			self._meshManipHelperView.postRedraw()



	def _o_getMesh(self):
		if self._meshCell is not None:
			return self._meshCell.getValueAsClass( GSProductMesh )
		else:
			return None

	def _o_reset(self):
		pass


	def _p_onMeshCellChanged(self):
		if self._meshManipHelperView is not None:
			self._o_reset()
