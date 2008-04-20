##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.UIAction import UIAction

from Britefury.ProceduralTool.ProcToolGroup import ProcToolGroup, ToolGroupActionInstanceProc, ToolGroupActionInstanceSubmenu, ToolGroupActionInstanceSignal

from Britefury.MeshTools.MeshEdit.Body.ToolMeshFlip import ProcMeshFlip
from Britefury.MeshTools.MeshEdit.Body.ToolMeshSubdivide import ProcMeshSubdivide
from Britefury.MeshTools.MeshEdit.Body.ToolMeshKnife import ProcMeshKnife
from Britefury.MeshTools.MeshEdit.Body.MeshAttachTool import MeshAttachTool
from Britefury.MeshTools.MeshEdit.Body.ToolMeshMirror import ProcMeshMirror
from Britefury.MeshTools.MeshEdit.Body.ToolMeshMirror import ProcMeshMirror
from Britefury.MeshTools.MeshEdit.ToolMeshDrawFace import ProcMeshDrawFace
from Britefury.MeshTools.MeshEdit.ToolMeshDrawQuads import ProcMeshDrawQuads



_flipAction = UIAction( 'Tools.Mesh.Edit.Body.Flip', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Body' ), _( 'Flip' ) ),
			_( 'Flip' ), _( 'Flip' ), None, _( 'Flip mesh' ) )
_subdivideAction = UIAction( 'Tools.Mesh.Edit.Body.Subdivide', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Body' ), _( 'Subdivide' ) ),
			_( 'Subdivide' ), _( 'Subdivide' ), 's', _( 'Subdivide mesh' ) )
_knifeAction = UIAction( 'Tools.Mesh.Edit.Body.Knife', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Body' ), _( 'Knife' ) ),
			_( 'Knife' ), _( 'Knife' ), 'k', _( 'Knife' ) )
_attachAction = UIAction( 'Tools.Mesh.Edit.Body.Attach', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Body' ), _( 'Attach' ) ),
			_( 'Attach' ), _( 'Attach' ), 'a', _( 'Attach' ) )

_drawFaceAction = UIAction( 'Tools.Mesh.Edit.Body.DrawFace', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Body' ), _( 'Draw face' ) ),
			_( 'Draw face' ), _( 'Draw face' ), 'f', _( 'Draw face' ) )
_drawQuadsAction = UIAction( 'Tools.Mesh.Edit.Body.DrawQuadMesh', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Body' ), _( 'Draw quad mesh' ) ),
			_( 'Draw quads' ), _( 'Draw quad mesh' ), 'm', _( 'Draw quad mesh' ) )

_mirrorAction = UIAction( 'Tools.Mesh.Edit.Body.Mirror', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Body' ), _( 'Mirror' ) ),
			_( 'Mirror' ), _( 'Mirror' ), None, _( 'Mirror; reflects in virtual mirror plane' ) )



class MeshEditBodyToolGroup (ProcToolGroup):
	def __init__(self, nodeEditor, sceneEditorTools, backgroundLayerPainter, view, parentWindow, toolPanel, commandHistory):
		super( MeshEditBodyToolGroup, self ).__init__( nodeEditor )

		self._sceneEditorTools = sceneEditorTools
		self._backgroundLayerPainter = backgroundLayerPainter
		self._view = view
		self._parentWindow = parentWindow
		self._commandHistory = commandHistory
		self._toolPanel = toolPanel

		self._flipAction = _flipAction.instance( ToolGroupActionInstanceProc, self, ProcMeshFlip )
		self._subdivideAction = _subdivideAction.instance( ToolGroupActionInstanceProc, self, ProcMeshSubdivide )
		self._knifeAction = _knifeAction.instance( ToolGroupActionInstanceProc, self, ProcMeshKnife )
		self._attachAction = _attachAction.instance( ToolGroupActionInstanceSignal, self )
		self._drawFaceAction = _drawFaceAction.instance( ToolGroupActionInstanceProc, self, ProcMeshDrawFace )
		self._drawQuadsAction = _drawQuadsAction.instance( ToolGroupActionInstanceProc, self, ProcMeshDrawQuads )

		self._mirrorAction = _mirrorAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMirror )

		self._attachAction.actionSignal.connect( self._p_onAttach )


	def layoutContents(self, layout):
		layout.row()  <<  _( 'General' )
		layout.row()  <<  self._subdivideAction.button()  <<  self._knifeAction.button()  <<  self._flipAction.button()
		layout.row()  <<  self._attachAction.button()  <<  None  <<  None
		layout.row()

		layout.row()  <<  _( 'Draw' )
		layout.row()  <<  self._drawFaceAction.button()  <<  self._drawQuadsAction.button()  <<  None
		layout.row()

		layout.row()  <<  _( 'Mirror' )
		layout.row()  <<  self._mirrorAction.button()  <<  None  <<  None
		layout.row()


	def initContextMenu(self, contextMenu):
		contextMenu.layout.row()  <<  self._subdivideAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._knifeAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._flipAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._attachAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._drawFaceAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._drawQuadsAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._mirrorAction.contextMenuItem( contextMenu )


	def _p_onAttach(self, actionInstance):
		if self._sceneEditorTools.scene is not None:
			attachTool = MeshAttachTool( self._sceneEditorTools, self._nodeEditor,self._sceneEditorTools.scene, self._backgroundLayerPainter, self._view, self._parentWindow, self._commandHistory )
			attachTool.install( self._toolPanel, self._parentWindow )
			attachTool.doneListener = self._p_onAttachDone

	def _p_onAttachDone(self, tool):
		tool.uninstall()
		tool.shutdownTool()
