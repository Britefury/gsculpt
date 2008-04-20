##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.UIAction import UIAction

from Britefury.ProceduralTool.ProcToolGroup import ProcToolGroup, ToolGroupActionInstanceProc, ToolGroupActionInstanceSubmenu, ToolGroupActionInstanceSignal

from Britefury.MeshTools.Mark.ToolMeshMarkPickFaces import ProcMeshMarkPickFaces
from Britefury.MeshTools.Mark.ToolMeshMarkRectangular import ProcMeshMarkRectangular
from Britefury.MeshTools.MeshEdit.Face.ToolMeshMarkFaces import ProcMeshMarkFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshGrowFaceSelection import ProcMeshGrowFaceSelection
from Britefury.MeshTools.MeshEdit.Face.ToolMeshShrinkFaceSelection import ProcMeshShrinkFaceSelection
from Britefury.MeshTools.MeshEdit.Face.ToolMeshMoveFaces import ProcMeshMoveFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshScaleFaces import ProcMeshScaleFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshRotateFaces import ProcMeshRotateFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshMoveFacesNormal import ProcMeshMoveFacesNormal
from Britefury.MeshTools.MeshEdit.Face.ToolMeshFlattenFaces import ProcMeshFlattenFaces, MeshFlattenFacesMode
from Britefury.MeshTools.MeshEdit.ToolMeshTweakComponents import ProcMeshTweakComponents
from Britefury.MeshTools.MeshEdit.ToolMeshSurfaceTweakComponents import ProcMeshSurfaceTweakComponents
from Britefury.MeshTools.MeshEdit.ToolMeshProportionalTweak import ProcMeshProportionalTweak
from Britefury.MeshTools.MeshEdit.Face.ToolMeshExtrudeFaces import ProcMeshExtrudeFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshFreeExtrudeFaces import ProcMeshFreeExtrudeFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshDuplicateFaces import ProcMeshDuplicateFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshInsetFaces import ProcMeshInsetFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshBevelFaces import ProcMeshBevelFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshExpandFaces import ProcMeshExpandFaces
from Britefury.MeshTools.MeshEdit.ToolMeshCut import ProcMeshCut
from Britefury.MeshTools.MeshEdit.ToolMeshNCut import ProcMeshNCut
from Britefury.MeshTools.MeshEdit.Face.ToolMeshWeldFaces import ProcMeshWeldFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshTunnel import ProcMeshTunnel
from Britefury.MeshTools.MeshEdit.Face.ToolMeshCollapseFaces import ProcMeshCollapseFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshDissolveFaces import ProcMeshDissolveFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshDeleteFaces import ProcMeshDeleteFaces
from Britefury.MeshTools.MeshEdit.Body.ToolMeshSubdivide import ProcMeshSubdivide

from Britefury.MeshTools.MeshEdit.Face.MeshDetachFacesOperation import meshDetachFaces

from Britefury.Model.Model import MarkPredicate

from Britefury.MeshTools.Mark.ToolMeshMarkRegion import MeshMarkRegionTarget
from Britefury.MeshTools.MeshEditComponent import MeshEditComponent

from Britefury.Mesh.Mesh import MeshTweakComponentTarget, MeshSurfaceTweakComponentTarget



_pickAction = UIAction( 'Tools.Mesh.Edit.Face.Pick', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Pick' ) ),
			_( 'Pick' ), _( 'Pick' ), None, _( 'Pick faces' ) )
_selectAction = UIAction( 'Tools.Mesh.Edit.Face.Select', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Select' ) ),
			_( 'Select' ), _( 'Select' ), '<shift>space', _( 'Select faces' ) )
_unselectAction = UIAction( 'Tools.Mesh.Edit.Face.Unselect', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Unselect' ) ),
			_( 'Unsel.' ), _( 'Unselect' ), 'space', _( 'Unselect faces' ) )
_regionSelAction = UIAction( 'Tools.Mesh.Edit.Face.RegionSelect', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Region select' ) ),
			_( 'Region' ), _( 'Region' ), 'z', _( 'Region select faces' ) )
_growAction = UIAction( 'Tools.Mesh.Edit.Face.Grow', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Grow selection' ) ),
			_( 'Grow' ), _( 'Grow' ), 'bracketright', _( 'Grow selection' ) )
_shrinkAction = UIAction( 'Tools.Mesh.Edit.Face.Shrink', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Shrink selection' ) ),
			_( 'Shrink' ), _( 'Shrink' ), 'bracketleft', _( 'Shrink selection' ) )
_moveAction = UIAction( 'Tools.Mesh.Edit.Face.Move', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Move' ) ),
			_( 'Move' ), _( 'Move' ), 'w', _( 'Move faces' ) )
_scaleAction = UIAction( 'Tools.Mesh.Edit.Face.Scale', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Scale' ) ),
			_( 'Scale' ), _( 'Scale' ), 'e', _( 'Scale faces' ) )
_rotateAction = UIAction( 'Tools.Mesh.Edit.Face.Rotate', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Rotate' ) ),
			_( 'Rotate' ), _( 'Rotate' ), 'r', _( 'Rotate faces' ) )
_moveNmlAction = UIAction( 'Tools.Mesh.Edit.Face.MoveNormal', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Move - normal' ) ),
			_( 'MoveNml.' ), _( 'Move normal' ), '<shift>w', _( 'Move faces normal' ) )
_flattenXAction = UIAction( 'Tools.Mesh.Edit.Face.Flatten.X', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Flatten' ), 'X' ),
			'X', 'X', None, _( 'Flatten faces - X' ) )
_flattenYAction = UIAction( 'Tools.Mesh.Edit.Face.Flatten.Y', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Flatten' ), 'Y' ),
			'Y', 'Y', None, _( 'Flatten faces - Y' ) )
_flattenZAction = UIAction( 'Tools.Mesh.Edit.Face.Flatten.Z', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Flatten' ), 'Z' ),
			'Z', 'Z', None, _( 'Flatten faces - Z' ) )
_flattenNmlAction = UIAction( 'Tools.Mesh.Edit.Face.Flatten.Normal', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Flatten' ), _( 'Normal' ) ),
			_( 'Normal' ), _( 'Normal' ), None, _( 'Flatten faces - Normal' ) )
_flattenMenuAction = UIAction( 'Tools.Mesh.Edit.Face.Flatten.PopupMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Flatten' ), _( 'Popup menu' ) ),
			_( 'Flatten' ), _( 'Flatten' ), 'f', _( 'Flatten faces' ) )
_tweakAction = UIAction( 'Tools.Mesh.Edit.Face.Tweak', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Tweak' ) ),
			_( 'Tweak' ), _( 'Tweak' ), 'q', _( 'Tweak components' ) )
_surfaceTweakAction = UIAction( 'Tools.Mesh.Edit.Face.SurfaceTweak', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Surface tweak' ) ),
			_( 'Surf Twk' ), _( 'Surface Tweak' ), '<shift>q', _( 'Surface tweak components' ) )
_proportionalTweakAction = UIAction( 'Tools.Mesh.Edit.Face.ProportionalTweak', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Proportional tweak' ) ),
			_( 'Prop Twk' ), _( 'Proportional tweak' ), 'a', _( 'Proportional tweak' ) )
_extrudeAction = UIAction( 'Tools.Mesh.Edit.Face.Extrude', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Extrude' ) ),
			_( 'Extrude' ), _( 'Extrude' ), 'x', _( 'Extrude faces' ) )
_freeExtrudeAction = UIAction( 'Tools.Mesh.Edit.Face.FreeExtrude', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Free extrude' ) ),
			_( 'F.Extrude' ), _( 'Free extrude' ), '<shift>x', _( 'Free extrude faces' ) )
_duplicateAction = UIAction( 'Tools.Mesh.Edit.Face.Duplicate', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Duplicate' ) ),
			_( 'Duplicate' ), _( 'Duplicate' ), '<shift>d', _( 'Duplicate faces' ) )
_insetAction = UIAction( 'Tools.Mesh.Edit.Face.Inset', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Inset faces' ) ),
			_( 'Inset' ), _( 'Inset' ), 'i', _( 'Inset faces' ) )
_insetFlowAction = UIAction( 'Tools.Mesh.Edit.Face.InsetWithFlow', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Inset with flow' ) ),
			_( 'InsetFlow' ), _( 'Inset with flow' ), '<shift>i', _( 'Inset faces, flow through marked edges' ) )
_bevelAction = UIAction( 'Tools.Mesh.Edit.Face.Bevel', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Bevel' ) ),
			_( 'Bevel' ), _( 'Bevel' ), 'b', _( 'Bevel faces' ) )
_bevelFlowAction = UIAction( 'Tools.Mesh.Edit.Face.BevelWithFlow', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Bevel with flow' ) ),
			_( 'BevelFlow' ), _( 'Bevel with flow' ), '<shift>b', _( 'Bevel faces, flow through marked edges' ) )
_expandAction = UIAction( 'Tools.Mesh.Edit.Face.Expand', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Expand' ) ),
			_( 'Expand' ), _( 'Expand' ), None, _( 'Expand faces' ) )
_cutAction = UIAction( 'Tools.Mesh.Edit.Face.Cut', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Cut' ) ),
			_( 'Cut' ), _( 'Cut' ), 't', _( 'Cut' ) )
_nCutAction = UIAction( 'Tools.Mesh.Edit.Face.NCut', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'N-Cut' ) ),
			_( 'NCut' ), _( 'N-Cut' ), '<shift>t', _( 'N-Cut' ) )
_weldAction = UIAction( 'Tools.Mesh.Edit.Face.Weld', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Weld' ) ),
			_( 'Weld' ), _( 'Weld' ), 'd', _( 'Weld' ) )
_tunnelAction = UIAction( 'Tools.Mesh.Edit.Face.Tunnel', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Tunnel' ) ),
			_( 'Tunnel' ), _( 'Tunnel' ), None, _( 'Tunnel' ) )
_collapseAction = UIAction( 'Tools.Mesh.Edit.Face.Collapse', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Collapse' ) ),
			_( 'Collapse' ), _( 'Collapse' ), 'o', _( 'Collapse faces' ) )
_dissolveAction = UIAction( 'Tools.Mesh.Edit.Face.Dissolve', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Dissolve' ) ),
			_( 'Dissolve' ), _( 'Dissolve' ), 'v', _( 'Dissolve faces' ) )
_deleteAction = UIAction( 'Tools.Mesh.Edit.Face.Delete', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Delete' ) ),
			_( 'Delete' ), _( 'Delete' ), 'Delete', _( 'Delete faces' ) )
_detachAction = UIAction( 'Tools.Mesh.Edit.Face.Detach', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Detach' ) ),
			_( 'Detach' ), _( 'Detach' ), 'slash', _( 'Detach faces' ) )
_subdivideAction = UIAction( 'Tools.Mesh.Edit.Face.Subdivide', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Subdivide' ) ),
			_( 'Subdivide' ), _( 'Subdivide' ), 's', _( 'Subdivide faces' ) )

_markMenuAction = UIAction( 'Tools.Mesh.Edit.Face.SelectMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Select menu' ) ),
			_( 'Select' ), _( 'Select' ), None, _( 'Selection tools' ) )
_moveMenuAction = UIAction( 'Tools.Mesh.Edit.Face.MoveMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Move / adjust menu' ) ),
			_( 'Move' ), _( 'Move/Adj.' ), None, _( 'Move / adjust tools' ) )
_expansionMenuAction = UIAction( 'Tools.Mesh.Edit.Face.ExpansionMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Expansion menu' ) ),
			_( 'Expansion' ), _( 'Expansion' ), None, _( 'Expansion tools' ) )
_cuttingMenuAction = UIAction( 'Tools.Mesh.Edit.Face.CuttingMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Cutting menu' ) ),
			_( 'Cutting' ), _( 'Cutting' ), None, _( 'Cutting tools' ) )
_joiningMenuAction = UIAction( 'Tools.Mesh.Edit.Face.JoiningMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Joining menu' ) ),
			_( 'Joining' ), _( 'Joining' ), None, _( 'Joining tools' ) )
_miscMenuAction = UIAction( 'Tools.Mesh.Edit.Face.MiscMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face' ), _( 'Misc menu' ) ),
			_( 'Misc.' ), _( 'Misc.' ), None, _( 'Misc. tools' ) )




class MeshEditFaceToolGroup (ProcToolGroup):
	def __init__(self, nodeEditor, sceneEditorTools, parentWindow, commandHistory):
		super( MeshEditFaceToolGroup, self ).__init__( nodeEditor )

		self._sceneEditorTools = sceneEditorTools
		self._parentWindow = parentWindow
		self._commandHistory = commandHistory

		def flattenFactory(mode):
			def f():
				proc = ProcMeshFlattenFaces()
				proc.mode = mode
				return proc
			return f

		def unmarkFactory():
			proc = ProcMeshMarkFaces()
			proc.predicate = MarkPredicate.UNMARK
			return proc

		def regionMarkFactory():
			proc = ProcMeshMarkRectangular()
			proc.target = MeshMarkRegionTarget.FACES
			return proc

		def insetFlowFactory():
			proc = ProcMeshInsetFaces()
			proc.bFlowThruMarkedEdges = True
			return proc

		def bevelFlowFactory():
			proc = ProcMeshBevelFaces()
			proc.bFlowThruMarkedEdges = True
			return proc

		def subdivideFactory():
			proc = ProcMeshSubdivide()
			proc.bMarkedFacesOnly = True
			return proc

		def tweakToolInitialiser(tool):
			tool.setTarget( MeshTweakComponentTarget.FACES )

		def surfaceTweakToolInitialiser(tool):
			tool.setTarget( MeshSurfaceTweakComponentTarget.FACES )

		self._pickAction = _pickAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMarkPickFaces )
		self._selectAction = _selectAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMarkFaces )
		self._unselectAction = _unselectAction.instance( ToolGroupActionInstanceProc, self, unmarkFactory )
		self._regionSelAction = _regionSelAction.instance( ToolGroupActionInstanceProc, self, regionMarkFactory )
		self._growAction = _growAction.instance( ToolGroupActionInstanceProc, self, ProcMeshGrowFaceSelection )
		self._shrinkAction = _shrinkAction.instance( ToolGroupActionInstanceProc, self, ProcMeshShrinkFaceSelection )
		self._moveAction = _moveAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMoveFaces )
		self._scaleAction = _scaleAction.instance( ToolGroupActionInstanceProc, self, ProcMeshScaleFaces )
		self._rotateAction = _rotateAction.instance( ToolGroupActionInstanceProc, self, ProcMeshRotateFaces )
		self._moveNmlAction = _moveNmlAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMoveFacesNormal )
		self._flattenXAction = _flattenXAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenFacesMode.X ) )
		self._flattenYAction = _flattenYAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenFacesMode.Y ) )
		self._flattenZAction = _flattenZAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenFacesMode.Z ) )
		self._flattenNmlAction = _flattenNmlAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenFacesMode.NORMAL ) )
		self._flattenMenuAction = _flattenMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenXAction.contextMenuItem( self._flattenMenuAction.menu )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenYAction.contextMenuItem( self._flattenMenuAction.menu )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenZAction.contextMenuItem( self._flattenMenuAction.menu )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenNmlAction.contextMenuItem( self._flattenMenuAction.menu )
		self._tweakAction = _tweakAction.instance( ToolGroupActionInstanceProc, self, ProcMeshTweakComponents, toolInitialiser=tweakToolInitialiser )
		self._surfaceTweakAction = _surfaceTweakAction.instance( ToolGroupActionInstanceProc, self, ProcMeshSurfaceTweakComponents, toolInitialiser=surfaceTweakToolInitialiser )
		self._proportionalTweakAction = _proportionalTweakAction.instance( ToolGroupActionInstanceProc, self, ProcMeshProportionalTweak )
		self._extrudeAction = _extrudeAction.instance( ToolGroupActionInstanceProc, self, ProcMeshExtrudeFaces )
		self._freeExtrudeAction = _freeExtrudeAction.instance( ToolGroupActionInstanceProc, self, ProcMeshFreeExtrudeFaces )
		self._duplicateAction = _duplicateAction.instance( ToolGroupActionInstanceProc, self, ProcMeshDuplicateFaces )
		self._insetAction = _insetAction.instance( ToolGroupActionInstanceProc, self, ProcMeshInsetFaces )
		self._insetFlowAction = _insetFlowAction.instance( ToolGroupActionInstanceProc, self, insetFlowFactory )
		self._bevelAction = _bevelAction.instance( ToolGroupActionInstanceProc, self, ProcMeshBevelFaces )
		self._bevelFlowAction = _bevelFlowAction.instance( ToolGroupActionInstanceProc, self, bevelFlowFactory )
		self._expandAction = _expandAction.instance( ToolGroupActionInstanceProc, self, ProcMeshExpandFaces )
		self._cutAction = _cutAction.instance( ToolGroupActionInstanceProc, self, ProcMeshCut )
		self._nCutAction = _nCutAction.instance( ToolGroupActionInstanceProc, self, ProcMeshNCut )
		self._weldAction = _weldAction.instance( ToolGroupActionInstanceProc, self, ProcMeshWeldFaces )
		self._tunnelAction = _tunnelAction.instance( ToolGroupActionInstanceProc, self, ProcMeshTunnel )
		self._collapseAction = _collapseAction.instance( ToolGroupActionInstanceProc, self, ProcMeshCollapseFaces )
		self._dissolveAction = _dissolveAction.instance( ToolGroupActionInstanceProc, self, ProcMeshDissolveFaces )
		self._deleteAction = _deleteAction.instance( ToolGroupActionInstanceProc, self, ProcMeshDeleteFaces )
		self._detachAction = _detachAction.instance( ToolGroupActionInstanceSignal, self )
		self._subdivideAction = _subdivideAction.instance( ToolGroupActionInstanceProc, self, subdivideFactory )

		self._markMenuAction = _markMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._moveMenuAction = _moveMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._expansionMenuAction = _expansionMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._cuttingMenuAction = _cuttingMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._joiningMenuAction = _joiningMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._miscMenuAction = _miscMenuAction.instance( ToolGroupActionInstanceSubmenu, self )


		self._detachAction.actionSignal.connect( self._p_onDetach )


		self._markMenuAction.menu.layout.row()  <<  self._pickAction.contextMenuItem( self._markMenuAction.menu )  <<		\
											self._selectAction.contextMenuItem( self._markMenuAction.menu )
		self._markMenuAction.menu.layout.row()  <<  self._regionSelAction.contextMenuItem( self._markMenuAction.menu )  <<		\
											self._unselectAction.contextMenuItem( self._markMenuAction.menu )
		self._markMenuAction.menu.layout.row()  <<  self._growAction.contextMenuItem( self._markMenuAction.menu )  <<		\
											self._shrinkAction.contextMenuItem( self._markMenuAction.menu )

		self._moveMenuAction.menu.layout.row()  <<  self._moveAction.contextMenuItem( self._moveMenuAction.menu )  <<	\
											self._scaleAction.contextMenuItem( self._moveMenuAction.menu )
		self._moveMenuAction.menu.layout.row()  <<  self._rotateAction.contextMenuItem( self._moveMenuAction.menu )  <<	\
											self._moveNmlAction.contextMenuItem( self._moveMenuAction.menu )
		self._moveMenuAction.menu.layout.row()  <<  self._flattenMenuAction.contextMenuItem( self._moveMenuAction.menu )  <<	\
											self._tweakAction.contextMenuItem( self._moveMenuAction.menu )
		self._moveMenuAction.menu.layout.row()  <<  self._surfaceTweakAction.contextMenuItem( self._moveMenuAction.menu )  <<		\
											self._proportionalTweakAction.contextMenuItem( self._moveMenuAction.menu )

		self._expansionMenuAction.menu.layout.row()  <<  self._extrudeAction.contextMenuItem( self._expansionMenuAction.menu )  <<	\
												self._freeExtrudeAction.contextMenuItem( self._expansionMenuAction.menu )
		self._expansionMenuAction.menu.layout.row()  <<  self._insetAction.contextMenuItem( self._expansionMenuAction.menu )  <<	\
												self._bevelAction.contextMenuItem( self._expansionMenuAction.menu )
		self._expansionMenuAction.menu.layout.row()  <<  self._bevelAction.contextMenuItem( self._expansionMenuAction.menu )  <<	\
												self._expandAction.contextMenuItem( self._expansionMenuAction.menu )

		self._cuttingMenuAction.menu.layout.row()  <<  self._cutAction.contextMenuItem( self._cuttingMenuAction.menu )  <<	\
											self._nCutAction.contextMenuItem( self._cuttingMenuAction.menu )

		self._joiningMenuAction.menu.layout.row()  <<  self._dissolveAction.contextMenuItem( self._joiningMenuAction.menu )  <<		\
											self._collapseAction.contextMenuItem( self._joiningMenuAction.menu )
		self._joiningMenuAction.menu.layout.row()  <<  self._weldAction.contextMenuItem( self._joiningMenuAction.menu )  <<	\
											self._tunnelAction.contextMenuItem( self._joiningMenuAction.menu )

		self._miscMenuAction.menu.layout.row()  <<  self._deleteAction.contextMenuItem( self._miscMenuAction.menu )  <<		\
											self._duplicateAction.contextMenuItem( self._miscMenuAction.menu )
		self._miscMenuAction.menu.layout.row()  <<  self._detachAction.contextMenuItem( self._miscMenuAction.menu )  <<		\
											self._subdivideAction.contextMenuItem( self._miscMenuAction.menu )


	def layoutContents(self, layout):
		layout.row()  <<  _( 'Select' )
		layout.row()  <<  self._pickAction.button()  <<  self._regionSelAction.button()  <<  self._selectAction.button()
		layout.row()  <<  self._growAction.button()  <<  self._shrinkAction.button()  <<  self._unselectAction.button()
		layout.row()
		layout.row()  <<  _( 'Move / Adjustment' )
		layout.row()  <<  self._moveAction.button()  <<  self._scaleAction.button()  <<  self._rotateAction.button()
		layout.row()  <<  self._moveNmlAction.button()  <<  self._flattenMenuAction.button()  <<  None
		layout.row()  <<  self._tweakAction.button()  <<  self._surfaceTweakAction.button()  <<  self._proportionalTweakAction.button()
		layout.row()
		layout.row()  <<  _( 'Expansion' )
		layout.row()  <<  self._extrudeAction.button()  <<  self._freeExtrudeAction.button()  <<  None
		layout.row()  <<  self._insetAction.button()  <<  self._bevelAction.button()  <<  self._expandAction.button()
		layout.row()
		layout.row()  <<  _( 'Joining' )
		layout.row()  <<  self._dissolveAction.button()  <<  self._collapseAction.button()  <<  self._weldAction.button()
		layout.row()  <<  self._tunnelAction.button()  <<  None  <<  None
		layout.row()
		layout.row()  <<  _( 'Misc.' )
		layout.row()  <<  self._deleteAction.button()  <<  self._duplicateAction.button()  <<  self._detachAction.button()
		layout.row()  <<  self._subdivideAction.button()  <<  None  <<  None
		layout.row()


	def initContextMenu(self, contextMenu):
		contextMenu.layout.row()  <<  self._markMenuAction.contextMenuItem( contextMenu )  <<  self._moveMenuAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._expansionMenuAction.contextMenuItem( contextMenu )  <<  self._cuttingMenuAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._joiningMenuAction.contextMenuItem( contextMenu )  <<  self._miscMenuAction.contextMenuItem( contextMenu )



	def _p_onDetach(self, action):
		meshDetachFaces( self._sceneEditorTools.currentNode, self._sceneEditorTools.scene, self._sceneEditorTools, self._nodeEditor, self._parentWindow, self._commandHistory )
