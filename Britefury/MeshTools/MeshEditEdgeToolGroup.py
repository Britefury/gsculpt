##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.UIAction import UIAction

from Britefury.ProceduralTool.ProcToolGroup import ProcToolGroup, ToolGroupActionInstanceProc, ToolGroupActionInstanceSubmenu

from Britefury.MeshTools.Mark.ToolMeshMarkPickEdges import ProcMeshMarkPickEdges
from Britefury.MeshTools.Mark.ToolMeshPickEdgeRingPath import ProcMeshPickEdgeRingPath
from Britefury.MeshTools.Mark.ToolMeshPickEdgeLoopPath import ProcMeshPickEdgeLoopPath
from Britefury.MeshTools.Mark.ToolMeshMarkRectangular import ProcMeshMarkRectangular
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshMarkEdges import ProcMeshMarkEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshGrowEdgeSelection import ProcMeshGrowEdgeSelection
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshShrinkEdgeSelection import ProcMeshShrinkEdgeSelection
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshSelectEdgeRing import ProcMeshSelectEdgeRing
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshSelectEdgeLoop import ProcMeshSelectEdgeLoop
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshSelectEdgeBoundary import ProcMeshSelectEdgeBoundary
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshMoveEdges import ProcMeshMoveEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshScaleEdges import ProcMeshScaleEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshRotateEdges import ProcMeshRotateEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshMoveEdgesNormal import ProcMeshMoveEdgesNormal
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshSlideEdges import ProcMeshSlideEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshFlattenEdges import ProcMeshFlattenEdges, MeshFlattenEdgesMode
from Britefury.MeshTools.MeshEdit.ToolMeshTweakComponents import ProcMeshTweakComponents
from Britefury.MeshTools.MeshEdit.ToolMeshSurfaceTweakComponents import ProcMeshSurfaceTweakComponents
from Britefury.MeshTools.MeshEdit.ToolMeshProportionalTweak import ProcMeshProportionalTweak
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshSpinEdges import ProcMeshSpinEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshRewireEdges import ProcMeshRewireEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshExpandEdges import ProcMeshExpandEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshExtrudeEdges import ProcMeshExtrudeEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshFillEdges import ProcMeshFillEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshRiftEdges import ProcMeshRiftEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshBevelEdges import ProcMeshBevelEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshConnectEdges import ProcMeshConnectEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshNConnectEdges import ProcMeshNConnectEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshSplitEdges import ProcMeshSplitEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshNSplitEdges import ProcMeshNSplitEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshDissolveEdges import ProcMeshDissolveEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshCollapseEdges import ProcMeshCollapseEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshWeldEdges import ProcMeshWeldEdges
from Britefury.MeshTools.MeshEdit.Edge.ToolMeshBridge import ProcMeshBridge
from Britefury.MeshTools.MeshEdit.ToolMeshCut import ProcMeshCut
from Britefury.MeshTools.MeshEdit.ToolMeshNCut import ProcMeshNCut
from Britefury.MeshTools.MeshEdit.ToolMeshBandsaw import ProcMeshBandsaw
from Britefury.MeshTools.MeshEdit.ToolMeshNBandsaw import ProcMeshNBandsaw
from Britefury.MeshTools.MeshEdit.ToolMeshCrease import ProcMeshCrease
from Britefury.MeshTools.MeshEdit.ToolMeshNormalSharpness import ProcMeshNormalSharpness

from Britefury.Model.Model import MarkPredicate

from Britefury.MeshTools.Mark.ToolMeshMarkRegion import MeshMarkRegionTarget
from Britefury.MeshTools.MeshEditComponent import MeshEditComponent

from Britefury.Mesh.Mesh import MeshTweakComponentTarget, MeshSurfaceTweakComponentTarget



_pickAction = UIAction( 'Tools.Mesh.Edit.Edge.Pick', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Pick' ) ),
			_( 'Pick' ), _( 'Pick' ), None, _( 'Pick edges' ) )
_pickRingPathAction = UIAction( 'Tools.Mesh.Edit.Edge.PickRingPath', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Pick ring path' ) ),
			_( 'RingPth.' ), _( 'Pick ring path' ), '<shift>g', _( 'Pick edge ring path' ) )
_pickLoopPathAction = UIAction( 'Tools.Mesh.Edit.Edge.PickLoopPath', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Pick loop path' ) ),
			_( 'LoopPth.' ), _( 'Pick loop path' ), '<shift>l', _( 'Pick edge loop path' ) )
_selectAction = UIAction( 'Tools.Mesh.Edit.Edge.Select', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Select' ) ),
			_( 'Select' ), _( 'Select' ), '<shift>space', _( 'Select edges' ) )
_unselectAction = UIAction( 'Tools.Mesh.Edit.Edge.Unselect', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Unselect' ) ),
			_( 'Unsel.' ), _( 'Unselect' ), 'space', _( 'Unselect edges' ) )
_regionSelAction = UIAction( 'Tools.Mesh.Edit.Edge.RegionSelect', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Region select' ) ),
			_( 'Region' ), _( 'Region' ), 'z', _( 'Region select edges' ) )
_growAction = UIAction( 'Tools.Mesh.Edit.Edge.Grow', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Grow selection' ) ),
			_( 'Grow' ), _( 'Grow' ), 'bracketright', _( 'Grow selection' ) )
_shrinkAction = UIAction( 'Tools.Mesh.Edit.Edge.Shrink', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Shrink selection' ) ),
			_( 'Shrink' ), _( 'Shrink' ), 'bracketleft', _( 'Shrink selection' ) )
_selRingAction = UIAction( 'Tools.Mesh.Edit.Edge.SelectRing', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Select rings' ) ),
			_( 'Ring' ), _( 'Select ring' ), None, _( 'Select edge ring' ) )
_selLoopAction = UIAction( 'Tools.Mesh.Edit.Edge.SelectLoop', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Select loops' ) ),
			_( 'Loop' ), _( 'Select loop' ), None, _( 'Select edge loop' ) )
_selBoundaryAction = UIAction( 'Tools.Mesh.Edit.Edge.SelectBoundary', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Select boundaries' ) ),
			_( 'Bound.' ), _( 'Select boundary' ), None, _( 'Select edge boundary' ) )
_moveAction = UIAction( 'Tools.Mesh.Edit.Edge.Move', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Move' ) ),
			_( 'Move' ), _( 'Move' ), 'w', _( 'Move edges' ) )
_scaleAction = UIAction( 'Tools.Mesh.Edit.Edge.Scale', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Scale' ) ),
			_( 'Scale' ), _( 'Scale' ), 'e', _( 'Scale edges' ) )
_rotateAction = UIAction( 'Tools.Mesh.Edit.Edge.Rotate', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Rotate' ) ),
			_( 'Rotate' ), _( 'Rotate' ), 'r', _( 'Rotate edges' ) )
_moveNmlAction = UIAction( 'Tools.Mesh.Edit.Edge.MoveNormal', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Move - normal' ) ),
			_( 'MoveNml.' ), _( 'Move normal' ), '<shift>w', _( 'Move edges normal' ) )
_slideAction = UIAction( 'Tools.Mesh.Edit.Edge.Slide', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Slide' ) ),
			_( 'Slide' ), _( 'Slide' ), '<shift>e', _( 'Slide edges' ) )
_flattenXAction = UIAction( 'Tools.Mesh.Edit.Edge.Flatten.X', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Flatten' ), 'X' ),
			'X', 'X', None, _( 'Flatten edges - X' ) )
_flattenYAction = UIAction( 'Tools.Mesh.Edit.Edge.Flatten.Y', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Flatten' ), 'Y' ),
			'Y', 'Y', None, _( 'Flatten edges - Y' ) )
_flattenZAction = UIAction( 'Tools.Mesh.Edit.Edge.Flatten.Z', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Flatten' ), 'Z' ),
			'Z', 'Z', None, _( 'Flatten edges - Z' ) )
_flattenNmlAction = UIAction( 'Tools.Mesh.Edit.Edge.Flatten.Normal', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Flatten' ), _( 'Normal' ) ),
			_( 'Normal' ), _( 'Normal' ), None, _( 'Flatten edges - Normal' ) )
_flattenFlattenRunsAction = UIAction( 'Tools.Mesh.Edit.Edge.Flatten.FlattenRuns', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Flatten' ), _( 'Flatten edge runs' ) ),
			_( 'Flatten runs' ), _( 'Flatten runs' ), None, _( 'Flatten edge runs' ) )
_flattenStraightenRunsAction = UIAction( 'Tools.Mesh.Edit.Edge.Flatten.StraightenRuns', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Flatten' ), _( 'Straighten edge runs' ) ),
			_( 'Straighten runs' ), _( 'Straighten runs' ), None, _( 'Straighten edge runs' ) )
_flattenMenuAction = UIAction( 'Tools.Mesh.Edit.Edge.Flatten.PopupMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Flatten' ), _( 'Popup menu' ) ),
			_( 'Flatten' ), _( 'Flatten' ), 'f', _( 'Flatten edges' ) )
_tweakAction = UIAction( 'Tools.Mesh.Edit.Edge.Tweak', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Tweak' ) ),
			_( 'Tweak' ), _( 'Tweak' ), 'q', _( 'Tweak components' ) )
_surfaceTweakAction = UIAction( 'Tools.Mesh.Edit.Edge.SurfaceTweak', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Surface tweak' ) ),
			_( 'Surf Twk' ), _( 'Surface Tweak' ), '<shift>q', _( 'Surface tweak components' ) )
_proportionalTweakAction = UIAction( 'Tools.Mesh.Edit.Edge.ProportionalTweak', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Proportional tweak' ) ),
			_( 'Prop Twk' ), _( 'Proportional tweak' ), 'a', _( 'Proportional tweak' ) )
_spinAction = UIAction( 'Tools.Mesh.Edit.Edge.Spin', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Spin' ) ),
			_( 'Spin' ), _( 'Spin' ), 'p', _( 'Spin edges' ) )
_rewireAction = UIAction( 'Tools.Mesh.Edit.Edge.Rewire', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Rewire' ) ),
			_( 'Rewire' ), _( 'Rewire' ), 'i', _( 'Rewire edges' ) )
_expandAction = UIAction( 'Tools.Mesh.Edit.Edge.Expand', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Expand' ) ),
			_( 'Expand' ), _( 'Expand' ), '<shift>x', _( 'Expand edges' ) )
_extrudeAction = UIAction( 'Tools.Mesh.Edit.Edge.Extrude', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Extrude' ) ),
			_( 'Extrude' ), _( 'Extrude' ), 'x', _( 'Extrude edges' ) )
_fillAction = UIAction( 'Tools.Mesh.Edit.Edge.Fill', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Fill' ) ),
			_( 'Fill' ), _( 'Fill' ), '<shift>f', _( 'Fill edges' ) )
_riftAction = UIAction( 'Tools.Mesh.Edit.Edge.Rift', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Rift' ) ),
			_( 'Rift' ), _( 'Rift' ), None, _( 'Rift edges' ) )
_bevelAction = UIAction( 'Tools.Mesh.Edit.Edge.Bevel', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Bevel' ) ),
			_( 'Bevel' ), _( 'Bevel' ), 'b', _( 'Bevel edges' ) )
_connectAction = UIAction( 'Tools.Mesh.Edit.Edge.Connect', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Connect' ) ),
			_( 'Connect' ), _( 'Connect' ), 'c', _( 'Connect edges' ) )
_nConnectAction = UIAction( 'Tools.Mesh.Edit.Edge.NConnect', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'N-Connect' ) ),
			_( 'N-Connect' ), _( 'N-Connect' ), '<shift>c', _( 'N-Connect edges' ) )
_splitAction = UIAction( 'Tools.Mesh.Edit.Edge.Split', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Split' ) ),
			_( 'Split' ), _( 'Split' ), 's', _( 'Split edges' ) )
_nSplitAction = UIAction( 'Tools.Mesh.Edit.Edge.NSplit', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'N-Split' ) ),
			_( 'N-Split' ), _( 'N-Split' ), '<shift>s', _( 'N-Split edges' ) )
_cutAction = UIAction( 'Tools.Mesh.Edit.Edge.Cut', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Cut' ) ),
			_( 'Cut' ), _( 'Cut' ), 't', _( 'Cut' ) )
_nCutAction = UIAction( 'Tools.Mesh.Edit.Edge.NCut', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'N-Cut' ) ),
			_( 'NCut' ), _( 'N-Cut' ), '<shift>t', _( 'N-Cut' ) )
_bandsawAction = UIAction( 'Tools.Mesh.Edit.Edge.Bandsaw', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Bandsaw' ) ),
			_( 'Bandsaw' ), _( 'Bandsaw' ), 'n', _( 'Bandsaw' ) )
_nBandsawAction = UIAction( 'Tools.Mesh.Edit.Edge.NBandsaw', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'N-Bandsaw' ) ),
			_( 'N-Bandsaw' ), _( 'N-Bandsaw' ), '<shift>n', _( 'N-Bandsaw' ) )
_dissolveAction = UIAction( 'Tools.Mesh.Edit.Edge.Dissolve', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Dissolve' ) ),
			_( 'Dissolve' ), _( 'Dissolve' ), 'v', _( 'Dissolve edges' ) )
_cleanDissolveAction = UIAction( 'Tools.Mesh.Edit.Edge.CleanDissolve', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Clean dissolve' ) ),
			_( 'Cln.Diss.' ), _( 'Clean dissolve' ), '<shift>v', _( 'Clean dissolve' ) )
_collapseAction = UIAction( 'Tools.Mesh.Edit.Edge.Collapse', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Collapse' ) ),
			_( 'Collapse' ), _( 'Collapse' ), 'o', _( 'Collapse edges' ) )
_weldAction = UIAction( 'Tools.Mesh.Edit.Edge.Weld', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Weld' ) ),
			_( 'Weld' ), _( 'Weld' ), 'd', _( 'Weld' ) )
_bridgeAction = UIAction( 'Tools.Mesh.Edit.Edge.Bridge', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Bridge' ) ),
			_( 'Bridge' ), _( 'Bridge' ), None, _( 'Bridge' ) )
_creaseAction = UIAction( 'Tools.Mesh.Edit.Edge.Crease', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Crease' ) ),
			_( 'Crease' ), _( 'Crease' ), None, _( 'Crease vertices' ) )
_nsharpAction = UIAction( 'Tools.Mesh.Edit.Edge.SharpenNormals', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Sharpen edge normals' ) ),
			_( 'Sharp' ), _( 'Sharp' ), None, _( 'Sharpen edge normals' ) )
_nsmoothAction = UIAction( 'Tools.Mesh.Edit.Edge.SmoothNormals', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Smooth edge normals' ) ),
			_( 'Smooth' ), _( 'Smooth' ), None, _( 'Smooth edge normals' ) )

_markMenuAction = UIAction( 'Tools.Mesh.Edit.Edge.SelectMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Select menu' ) ),
			_( 'Select' ), _( 'Select' ), None, _( 'Selection tools' ) )
_moveMenuAction = UIAction( 'Tools.Mesh.Edit.Edge.MoveMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Move menu' ) ),
			_( 'Move' ), _( 'Move/Adj.' ), None, _( 'Move / adjust tools' ) )
_expansionMenuAction = UIAction( 'Tools.Mesh.Edit.Edge.ExpansionMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Expansion menu' ) ),
			_( 'Expansion' ), _( 'Expansion' ), None, _( 'Expansion tools' ) )
_cuttingMenuAction = UIAction( 'Tools.Mesh.Edit.Edge.CuttingMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Cutting menu' ) ),
			_( 'Cutting' ), _( 'Cutting' ), None, _( 'Cutting tools' ) )
_joiningMenuAction = UIAction( 'Tools.Mesh.Edit.Edge.JoiningMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Joining menu' ) ),
			_( 'Joining' ), _( 'Joining' ), None, _( 'Joining tools' ) )
_creaseMenuAction = UIAction( 'Tools.Mesh.Edit.Edge.CreaseMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge' ), _( 'Crease menu' ) ),
			_( 'Crease' ), _( 'Crease' ), None, _( 'Crease tools' ) )




class MeshEditEdgeToolGroup (ProcToolGroup):
	def __init__(self, nodeEditor):
		super( MeshEditEdgeToolGroup, self ).__init__( nodeEditor )

		def flattenFactory(mode):
			def f():
				proc = ProcMeshFlattenEdges()
				proc.mode = mode
				return proc
			return f

		def unmarkFactory():
			proc = ProcMeshMarkEdges()
			proc.predicate = MarkPredicate.UNMARK
			return proc

		def regionMarkFactory():
			proc = ProcMeshMarkRectangular()
			proc.target = MeshMarkRegionTarget.EDGES
			return proc

		def cleanDissolveFactory():
			proc = ProcMeshDissolveEdges()
			proc.bDissolveSimpleValenceTwoVertices = True
			return proc

		def creaseFactory():
			proc = ProcMeshCrease()
			proc.targetComponent = MeshEditComponent.EDGES
			return proc

		def sharpenFactory():
			proc = ProcMeshNormalSharpness()
			proc.targetComponent = MeshEditComponent.EDGES
			proc.sharpness = True
			return proc

		def smoothFactory():
			proc = ProcMeshNormalSharpness()
			proc.targetComponent = MeshEditComponent.EDGES
			proc.sharpness = False
			return proc

		def tweakToolInitialiser(tool):
			tool.setTarget( MeshTweakComponentTarget.EDGES )

		def surfaceTweakToolInitialiser(tool):
			tool.setTarget( MeshSurfaceTweakComponentTarget.EDGES )

		self._pickAction = _pickAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMarkPickEdges )
		self._pickRingPathAction = _pickRingPathAction.instance( ToolGroupActionInstanceProc, self, ProcMeshPickEdgeRingPath )
		self._pickLoopPathAction = _pickLoopPathAction.instance( ToolGroupActionInstanceProc, self, ProcMeshPickEdgeLoopPath )
		self._selectAction = _selectAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMarkEdges )
		self._unselectAction = _unselectAction.instance( ToolGroupActionInstanceProc, self, unmarkFactory )
		self._regionSelAction = _regionSelAction.instance( ToolGroupActionInstanceProc, self, regionMarkFactory )
		self._growAction = _growAction.instance( ToolGroupActionInstanceProc, self, ProcMeshGrowEdgeSelection )
		self._shrinkAction = _shrinkAction.instance( ToolGroupActionInstanceProc, self, ProcMeshShrinkEdgeSelection )
		self._selRingAction = _selRingAction.instance( ToolGroupActionInstanceProc, self, ProcMeshSelectEdgeRing )
		self._selLoopAction = _selLoopAction.instance( ToolGroupActionInstanceProc, self, ProcMeshSelectEdgeLoop )
		self._selBoundaryAction = _selBoundaryAction.instance( ToolGroupActionInstanceProc, self, ProcMeshSelectEdgeBoundary )
		self._moveAction = _moveAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMoveEdges )
		self._scaleAction = _scaleAction.instance( ToolGroupActionInstanceProc, self, ProcMeshScaleEdges )
		self._rotateAction = _rotateAction.instance( ToolGroupActionInstanceProc, self, ProcMeshRotateEdges )
		self._moveNmlAction = _moveNmlAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMoveEdgesNormal )
		self._slideAction = _slideAction.instance( ToolGroupActionInstanceProc, self, ProcMeshSlideEdges )
		self._flattenXAction = _flattenXAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenEdgesMode.X ) )
		self._flattenYAction = _flattenYAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenEdgesMode.Y ) )
		self._flattenZAction = _flattenZAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenEdgesMode.Z ) )
		self._flattenNmlAction = _flattenNmlAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenEdgesMode.NORMAL ) )
		self._flattenFlattenRunsAction = _flattenFlattenRunsAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenEdgesMode.FLATTEN_RUNS ) )
		self._flattenStraightenRunsAction = _flattenStraightenRunsAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenEdgesMode.STRAIGHTEN_RUNS ) )
		self._flattenMenuAction = _flattenMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenXAction.contextMenuItem( self._flattenMenuAction.menu )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenYAction.contextMenuItem( self._flattenMenuAction.menu )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenZAction.contextMenuItem( self._flattenMenuAction.menu )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenNmlAction.contextMenuItem( self._flattenMenuAction.menu )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenFlattenRunsAction.contextMenuItem( self._flattenMenuAction.menu )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenStraightenRunsAction.contextMenuItem( self._flattenMenuAction.menu )
		self._tweakAction = _tweakAction.instance( ToolGroupActionInstanceProc, self, ProcMeshTweakComponents, toolInitialiser=tweakToolInitialiser )
		self._surfaceTweakAction = _surfaceTweakAction.instance( ToolGroupActionInstanceProc, self, ProcMeshSurfaceTweakComponents, toolInitialiser=surfaceTweakToolInitialiser )
		self._proportionalTweakAction = _proportionalTweakAction.instance( ToolGroupActionInstanceProc, self, ProcMeshProportionalTweak )
		self._spinAction = _spinAction.instance( ToolGroupActionInstanceProc, self, ProcMeshSpinEdges )
		self._rewireAction = _rewireAction.instance( ToolGroupActionInstanceProc, self, ProcMeshRewireEdges )
		self._expandAction = _expandAction.instance( ToolGroupActionInstanceProc, self, ProcMeshExpandEdges )
		self._extrudeAction = _extrudeAction.instance( ToolGroupActionInstanceProc, self, ProcMeshExtrudeEdges )
		self._fillAction = _fillAction.instance( ToolGroupActionInstanceProc, self, ProcMeshFillEdges )
		self._riftAction = _riftAction.instance( ToolGroupActionInstanceProc, self, ProcMeshRiftEdges )
		self._bevelAction = _bevelAction.instance( ToolGroupActionInstanceProc, self, ProcMeshBevelEdges )
		self._connectAction = _connectAction.instance( ToolGroupActionInstanceProc, self, ProcMeshConnectEdges )
		self._nConnectAction = _nConnectAction.instance( ToolGroupActionInstanceProc, self, ProcMeshNConnectEdges )
		self._splitAction = _splitAction.instance( ToolGroupActionInstanceProc, self, ProcMeshSplitEdges )
		self._nSplitAction = _nSplitAction.instance( ToolGroupActionInstanceProc, self, ProcMeshNSplitEdges )
		self._cutAction = _cutAction.instance( ToolGroupActionInstanceProc, self, ProcMeshCut )
		self._nCutAction = _nCutAction.instance( ToolGroupActionInstanceProc, self, ProcMeshNCut )
		self._bandsawAction = _bandsawAction.instance( ToolGroupActionInstanceProc, self, ProcMeshBandsaw )
		self._nBandsawAction = _nBandsawAction.instance( ToolGroupActionInstanceProc, self, ProcMeshNBandsaw )
		self._dissolveAction = _dissolveAction.instance( ToolGroupActionInstanceProc, self, ProcMeshDissolveEdges )
		self._cleanDissolveAction = _cleanDissolveAction.instance( ToolGroupActionInstanceProc, self, cleanDissolveFactory )
		self._collapseAction = _collapseAction.instance( ToolGroupActionInstanceProc, self, ProcMeshCollapseEdges )
		self._weldAction = _weldAction.instance( ToolGroupActionInstanceProc, self, ProcMeshWeldEdges )
		self._bridgeAction = _bridgeAction.instance( ToolGroupActionInstanceProc, self, ProcMeshBridge )
		self._creaseAction = _creaseAction.instance( ToolGroupActionInstanceProc, self, creaseFactory )
		self._nsharpAction = _nsharpAction.instance( ToolGroupActionInstanceProc, self, sharpenFactory )
		self._nsmoothAction = _nsmoothAction.instance( ToolGroupActionInstanceProc, self, smoothFactory )

		self._markMenuAction = _markMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._moveMenuAction = _moveMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._expansionMenuAction = _expansionMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._cuttingMenuAction = _cuttingMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._joiningMenuAction = _joiningMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._creaseMenuAction = _creaseMenuAction.instance( ToolGroupActionInstanceSubmenu, self )


		self._markMenuAction.menu.layout.row()  <<  self._pickAction.contextMenuItem( self._markMenuAction.menu )  <<		\
											self._selectAction.contextMenuItem( self._markMenuAction.menu )
		self._markMenuAction.menu.layout.row()  <<  self._regionSelAction.contextMenuItem( self._markMenuAction.menu )  <<		\
											self._unselectAction.contextMenuItem( self._markMenuAction.menu )
		self._markMenuAction.menu.layout.row()  <<  self._growAction.contextMenuItem( self._markMenuAction.menu )  <<		\
											self._shrinkAction.contextMenuItem( self._markMenuAction.menu )
		self._markMenuAction.menu.layout.row()  <<  self._pickRingPathAction.contextMenuItem( self._markMenuAction.menu )  <<		\
											self._pickLoopPathAction.contextMenuItem( self._markMenuAction.menu )
		self._markMenuAction.menu.layout.row()  <<  self._selRingAction.contextMenuItem( self._markMenuAction.menu )  <<		\
											self._selLoopAction.contextMenuItem( self._markMenuAction.menu )
		self._markMenuAction.menu.layout.row()  <<  self._selBoundaryAction.contextMenuItem( self._markMenuAction.menu )  <<  None


		self._moveMenuAction.menu.layout.row()  <<  self._moveAction.contextMenuItem( self._moveMenuAction.menu )  <<	\
											self._scaleAction.contextMenuItem( self._moveMenuAction.menu )
		self._moveMenuAction.menu.layout.row()  <<  self._rotateAction.contextMenuItem( self._moveMenuAction.menu )  <<	\
											self._moveNmlAction.contextMenuItem( self._moveMenuAction.menu )
		self._moveMenuAction.menu.layout.row()  <<  self._slideAction.contextMenuItem( self._moveMenuAction.menu )  <<		\
											self._flattenMenuAction.contextMenuItem( self._moveMenuAction.menu )
		self._moveMenuAction.menu.layout.row()  <<  self._tweakAction.contextMenuItem( self._moveMenuAction.menu )  <<	\
											self._surfaceTweakAction.contextMenuItem( self._moveMenuAction.menu )
		self._moveMenuAction.menu.layout.row()  <<  self._spinAction.contextMenuItem( self._moveMenuAction.menu )  <<		\
											self._rewireAction.contextMenuItem( self._moveMenuAction.menu )
		self._moveMenuAction.menu.layout.row()  <<  self._proportionalTweakAction.contextMenuItem( self._moveMenuAction.menu )  <<  None

		self._expansionMenuAction.menu.layout.row()  <<  self._expandAction.contextMenuItem( self._expansionMenuAction.menu )  <<	\
												self._extrudeAction.contextMenuItem( self._expansionMenuAction.menu )
		self._expansionMenuAction.menu.layout.row()  <<  self._bevelAction.contextMenuItem( self._expansionMenuAction.menu )  <<		\
												self._riftAction.contextMenuItem( self._expansionMenuAction.menu )
		self._expansionMenuAction.menu.layout.row()  <<  self._fillAction.contextMenuItem( self._expansionMenuAction.menu )  <<		\
												None

		self._cuttingMenuAction.menu.layout.row()  <<  self._connectAction.contextMenuItem( self._cuttingMenuAction.menu )  <<		\
											self._nConnectAction.contextMenuItem( self._cuttingMenuAction.menu )
		self._cuttingMenuAction.menu.layout.row()  <<  self._splitAction.contextMenuItem( self._cuttingMenuAction.menu )  <<	\
											self._nSplitAction.contextMenuItem( self._cuttingMenuAction.menu )
		self._cuttingMenuAction.menu.layout.row()  <<  self._cutAction.contextMenuItem( self._cuttingMenuAction.menu )  <<	\
											self._nCutAction.contextMenuItem( self._cuttingMenuAction.menu )
		self._cuttingMenuAction.menu.layout.row()  <<  self._bandsawAction.contextMenuItem( self._cuttingMenuAction.menu )  <<	\
											self._nBandsawAction.contextMenuItem( self._cuttingMenuAction.menu )

		self._joiningMenuAction.menu.layout.row()  <<  self._dissolveAction.contextMenuItem( self._joiningMenuAction.menu )  <<		\
											self._collapseAction.contextMenuItem( self._joiningMenuAction.menu )
		self._joiningMenuAction.menu.layout.row()  <<  self._weldAction.contextMenuItem( self._joiningMenuAction.menu )  <<	\
											self._bridgeAction.contextMenuItem( self._joiningMenuAction.menu )

		self._creaseMenuAction.menu.layout.row()  <<  self._creaseAction.contextMenuItem( self._creaseMenuAction.menu )  <<  None
		self._creaseMenuAction.menu.layout.row()  <<  self._nsharpAction.contextMenuItem( self._creaseMenuAction.menu )  <<		\
												self._nsmoothAction.contextMenuItem( self._creaseMenuAction.menu )


	def layoutContents(self, layout):
		layout.row()  <<  _( 'Select' )
		layout.row()  <<  self._pickAction.button()  <<  self._regionSelAction.button()  <<  self._selectAction.button()
		layout.row()  <<  self._growAction.button()  <<  self._shrinkAction.button()  <<  self._unselectAction.button()
		layout.row()  <<  self._selLoopAction.button()  <<  self._selRingAction.button()  <<  self._selBoundaryAction.button()
		layout.row()  <<  self._pickLoopPathAction.button()  <<  self._pickRingPathAction.button()  <<  None
		layout.row()
		layout.row()  <<  _( 'Move / Adjustment' )
		layout.row()  <<  self._moveAction.button()  <<  self._scaleAction.button()  <<  self._rotateAction.button()
		layout.row()  <<  self._moveNmlAction.button()  <<  self._slideAction.button()  <<  self._flattenMenuAction.button()
		layout.row()  <<  self._tweakAction.button()  <<  self._surfaceTweakAction.button()  <<  self._proportionalTweakAction.button()
		layout.row()  <<  self._spinAction.button()  <<  self._rewireAction.button()  <<  None
		layout.row()
		layout.row()  <<  _( 'Expansion' )
		layout.row()  <<  self._expandAction.button()  <<  self._extrudeAction.button()  <<  self._riftAction.button()
		layout.row()  <<  self._bevelAction.button()  <<  self._fillAction.button()  <<  None
		layout.row()
		layout.row()  <<  _( 'Cutting' )
		layout.row()  <<  self._connectAction.button()  <<  self._nConnectAction.button()  <<  self._splitAction.button()
		layout.row()  <<  self._cutAction.button()  <<  self._nCutAction.button()  <<  self._nSplitAction.button()
		layout.row()  <<  self._bandsawAction.button()  <<  self._nBandsawAction.button()  <<  None
		layout.row()
		layout.row()  <<  _( 'Joining' )
		layout.row()  <<  self._dissolveAction.button()  <<  self._collapseAction.button()  <<  self._weldAction.button()
		layout.row()  <<  self._bridgeAction.button()  <<  None  <<  None
		layout.row()
		layout.row()  <<  _( 'Sharpness' )
		layout.row()  <<  self._creaseAction.button()  <<  self._nsharpAction.button()  <<  self._nsmoothAction.button()
		layout.row()


	def initContextMenu(self, contextMenu):
		contextMenu.layout.row()  <<  self._markMenuAction.contextMenuItem( contextMenu )  <<  self._moveMenuAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._expansionMenuAction.contextMenuItem( contextMenu )  <<  self._cuttingMenuAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._joiningMenuAction.contextMenuItem( contextMenu )  <<  self._creaseMenuAction.contextMenuItem( contextMenu )


