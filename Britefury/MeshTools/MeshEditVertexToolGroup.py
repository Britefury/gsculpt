##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.UIAction import UIAction

from Britefury.ProceduralTool.ProcToolGroup import ProcToolGroup, ToolGroupActionInstanceProc, ToolGroupActionInstanceSubmenu

from Britefury.MeshTools.Mark.ToolMeshMarkPickVertices import ProcMeshMarkPickVertices
from Britefury.MeshTools.Mark.ToolMeshMarkRectangular import ProcMeshMarkRectangular
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshMarkVertices import ProcMeshMarkVertices
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshGrowVertexSelection import ProcMeshGrowVertexSelection
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshShrinkVertexSelection import ProcMeshShrinkVertexSelection
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshMoveVertices import ProcMeshMoveVertices
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshScaleVertices import ProcMeshScaleVertices
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshRotateVertices import ProcMeshRotateVertices
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshMoveVerticesNormal import ProcMeshMoveVerticesNormal
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshFlattenVertices import ProcMeshFlattenVertices, MeshFlattenVerticesMode
from Britefury.MeshTools.MeshEdit.ToolMeshTweakComponents import ProcMeshTweakComponents
from Britefury.MeshTools.MeshEdit.ToolMeshSurfaceTweakComponents import ProcMeshSurfaceTweakComponents
from Britefury.MeshTools.MeshEdit.ToolMeshProportionalTweak import ProcMeshProportionalTweak
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshTemplateTweakVertices import ProcMeshTemplateTweakVertices
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshExpandVertices import ProcMeshExpandVertices
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshBevelVertices import ProcMeshBevelVertices
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshConnectVertices import ProcMeshConnectVertices
from Britefury.MeshTools.MeshEdit.ToolMeshCut import ProcMeshCut
from Britefury.MeshTools.MeshEdit.ToolMeshNCut import ProcMeshNCut
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshWeldVertices import ProcMeshWeldVertices
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshTargetWeldVertices import ProcMeshTargetWeldVertices
from Britefury.MeshTools.MeshEdit.Vertex.ToolMeshDissolveVertices import ProcMeshDissolveVertices
from Britefury.MeshTools.MeshEdit.ToolMeshCrease import ProcMeshCrease
from Britefury.MeshTools.MeshEdit.ToolMeshNormalSharpness import ProcMeshNormalSharpness

from Britefury.Model.Model import MarkPredicate

from Britefury.MeshTools.Mark.ToolMeshMarkRegion import MeshMarkRegionTarget
from Britefury.MeshTools.MeshEditComponent import MeshEditComponent

from Britefury.Mesh.Mesh import MeshTweakComponentTarget, MeshSurfaceTweakComponentTarget




_pickAction = UIAction( 'Tools.Mesh.Edit.Vertex.Pick', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Pick' ) ),
			_( 'Pick' ), _( 'Pick' ), None, _( 'Pick vertices' ) )
_markAction = UIAction( 'Tools.Mesh.Edit.Vertex.Select', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Select' ) ),
			_( 'Select' ), _( 'Select' ), '<shift>space', _( 'Select vertices' ) )
_unmarkAction = UIAction( 'Tools.Mesh.Edit.Vertex.Unselect', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Unselect' ) ),
			_( 'Unsel.' ), _( 'Unselect' ), 'space', _( 'Unselect vertices' ) )
_regionSelAction = UIAction( 'Tools.Mesh.Edit.Vertex.RegionSelect', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Region select' ) ),
			_( 'Region' ), _( 'Region' ), 'z', _( 'Region select vertices' ) )
_growAction = UIAction( 'Tools.Mesh.Edit.Vertex.Grow', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Grow selection' ) ),
			_( 'Grow' ), _( 'Grow' ), 'bracketright', _( 'Grow selection' ) )
_shrinkAction = UIAction( 'Tools.Mesh.Edit.Vertex.Shrink', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Shrink selection' ) ),
			_( 'Shrink' ), _( 'Shrink' ), 'bracketleft', _( 'Shrink selection' ) )
_moveAction = UIAction( 'Tools.Mesh.Edit.Vertex.Move', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Move' ) ),
			_( 'Move' ), _( 'Move' ), 'w', _( 'Move vertices' ) )
_scaleAction = UIAction( 'Tools.Mesh.Edit.Vertex.Scale', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Scale' ) ),
			_( 'Scale' ), _( 'Scale' ), 'e', _( 'Scale vertices' ) )
_rotateAction = UIAction( 'Tools.Mesh.Edit.Vertex.Rotate', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Rotate' ) ),
			_( 'Rotate' ), _( 'Rotate' ), 'r', _( 'Rotate vertices' ) )
_moveNmlAction = UIAction( 'Tools.Mesh.Edit.Vertex.MoveNormal', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'MoveNormal' ) ),
			_( 'MoveNml.' ), _( 'Move normal' ), '<shift>w', _( 'Move vertices normal' ) )
_flattenXAction = UIAction( 'Tools.Mesh.Edit.Vertex.Flatten.X', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Flatten' ), 'X' ),
			'X', 'X', None, _( 'Flatten vertices - X' ) )
_flattenYAction = UIAction( 'Tools.Mesh.Edit.Vertex.Flatten.Y', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Flatten' ), 'Y' ),
			'Y', 'Y', None, _( 'Flatten vertices - Y' ) )
_flattenZAction = UIAction( 'Tools.Mesh.Edit.Vertex.Flatten.Z', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Flatten' ), 'Z' ),
			'Z', 'Z', None, _( 'Flatten vertices - Z' ) )
_flattenNmlAction = UIAction( 'Tools.Mesh.Edit.Vertex.Flatten.Normal', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Flatten' ), _( 'Normal' ) ),
			_( 'Normal' ), _( 'Normal' ), None, _( 'Flatten vertices - Normal' ) )
_flattenMenuAction = UIAction( 'Tools.Mesh.Edit.Vertex.Flatten.PopupMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Flatten' ), _( 'Popup menu' ) ),
			_( 'Flatten' ), _( 'Flatten' ), 'f', _( 'Flatten vertices' ) )
_tweakAction = UIAction( 'Tools.Mesh.Edit.Vertex.Tweak', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Tweak' ) ),
			_( 'Tweak' ), _( 'Tweak' ), 'q', _( 'Tweak components' ) )
_surfaceTweakAction = UIAction( 'Tools.Mesh.Edit.Vertex.SurfaceTweak', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Surface tweak' ) ),
			_( 'Surf Twk' ), _( 'Surface tweak' ), '<shift>q', _( 'Surface tweak components' ) )
_templateTweakAction = UIAction( 'Tools.Mesh.Edit.Vertex.TemplateTweak', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Template tweak' ) ),
			_( 'Tmpl Twk' ), _( 'Template tweak' ), '<shift>a', _( 'Template tweak vertices' ) )
_proportionalTweakAction = UIAction( 'Tools.Mesh.Edit.Vertex.ProportionalTweak', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Proportional tweak' ) ),
			_( 'Prop Twk' ), _( 'Proportional tweak' ), 'a', _( 'Proportional tweak' ) )
_expandAction = UIAction( 'Tools.Mesh.Edit.Vertex.Expand', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Expand' ) ),
			_( 'Expand' ), _( 'Expand' ), 'x', _( 'Expand vertices' ) )
_bevelAction = UIAction( 'Tools.Mesh.Edit.Vertex.Bevel', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Bevel' ) ),
			_( 'Bevel' ), _( 'Bevel' ), 'b', _( 'Bevel vertices' ) )
_connectAction = UIAction( 'Tools.Mesh.Edit.Vertex.Connect', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Connect' ) ),
			_( 'Connect' ), _( 'Connect' ), 'c', _( 'Connect vertices' ) )
_cutAction = UIAction( 'Tools.Mesh.Edit.Vertex.Cut', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Cut' ) ),
			_( 'Cut' ), _( 'Cut' ), 't', _( 'Cut' ) )
_nCutAction = UIAction( 'Tools.Mesh.Edit.Vertex.NCut', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'N-Cut' ) ),
			_( 'NCut' ), _( 'N-Cut' ), '<shift>t', _( 'N-Cut' ) )
_weldAction = UIAction( 'Tools.Mesh.Edit.Vertex.Weld', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Weld' ) ),
			_( 'Weld' ), _( 'Weld' ), '<shift>d', _( 'Weld vertices' ) )
_targetWeldAction = UIAction( 'Tools.Mesh.Edit.Vertex.TargetWeld', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Target weld' ) ),
			_( 'Tgt.Weld' ), _( 'Target weld' ), 'd', _( 'Target weld vertices' ) )
_dissolveAction = UIAction( 'Tools.Mesh.Edit.Vertex.Dissolve', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Dissolve' ) ),
			_( 'Dissolve' ), _( 'Dissolve' ), 'v', _( 'Dissolve vertices' ) )
_creaseAction = UIAction( 'Tools.Mesh.Edit.Vertex.Crease', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Crease' ) ),
			_( 'Crease' ), _( 'Crease' ), None, _( 'Crease vertices' ) )
_nsharpAction = UIAction( 'Tools.Mesh.Edit.Vertex.SharpenNormals', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Sharpen vertex normals' ) ),
			_( 'Sharp' ), _( 'Sharp' ), None, _( 'Sharpen vertex normals' ) )
_nsmoothAction = UIAction( 'Tools.Mesh.Edit.Vertex.SmoothNormals', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Smooth vertex normals' ) ),
			_( 'Smooth' ), _( 'Smooth' ), None, _( 'Smooth vertex normals' ) )

_markMenuAction = UIAction( 'Tools.Mesh.Edit.Vertex.SelectMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Select menu' ) ),
			_( 'Select' ), _( 'Select' ), None, _( 'Selection tools' ) )
_moveMenuAction = UIAction( 'Tools.Mesh.Edit.Vertex.MoveMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Move / adjustment menu' ) ),
			_( 'Move' ), _( 'Move/Adj.' ), None, _( 'Move / adjust tools' ) )
_expansionMenuAction = UIAction( 'Tools.Mesh.Edit.Vertex.ExpansionMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Expansion menu' ) ),
			_( 'Expansion' ), _( 'Expansion' ), None, _( 'Expansion tools' ) )
_cuttingMenuAction = UIAction( 'Tools.Mesh.Edit.Vertex.CuttingMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Cutting menu' ) ),
			_( 'Cutting' ), _( 'Cutting' ), None, _( 'Cutting tools' ) )
_joiningMenuAction = UIAction( 'Tools.Mesh.Edit.Vertex.JoiningMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Joining menu' ) ),
			_( 'Joining' ), _( 'Joining' ), None, _( 'Joining tools' ) )
_creaseMenuAction = UIAction( 'Tools.Mesh.Edit.Vertex.CreaseMenu', ( _( 'Tools' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex' ), _( 'Crease menu' ) ),
			_( 'Crease' ), _( 'Crease' ), None, _( 'Crease tools' ) )





class MeshEditVertexToolGroup (ProcToolGroup):
	def __init__(self, nodeEditor):
		super( MeshEditVertexToolGroup, self ).__init__( nodeEditor )

		def flattenFactory(mode):
			def f():
				proc = ProcMeshFlattenVertices()
				proc.mode = mode
				proc.bPerRegion = False
				return proc
			return f

		def unmarkFactory():
			proc = ProcMeshMarkVertices()
			proc.predicate = MarkPredicate.UNMARK
			return proc

		def regionMarkFactory():
			proc = ProcMeshMarkRectangular()
			proc.target = MeshMarkRegionTarget.VERTICES
			return proc

		def creaseFactory():
			proc = ProcMeshCrease()
			proc.targetComponent = MeshEditComponent.VERTICES
			return proc

		def sharpenFactory():
			proc = ProcMeshNormalSharpness()
			proc.targetComponent = MeshEditComponent.VERTICES
			proc.sharpness = True
			return proc

		def smoothFactory():
			proc = ProcMeshNormalSharpness()
			proc.targetComponent = MeshEditComponent.VERTICES
			proc.sharpness = False
			return proc

		def tweakToolInitialiser(tool):
			tool.setTarget( MeshTweakComponentTarget.VERTICES )

		def surfaceTweakToolInitialiser(tool):
			tool.setTarget( MeshSurfaceTweakComponentTarget.VERTICES )

		self._pickAction = _pickAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMarkPickVertices )
		self._markAction = _markAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMarkVertices )
		self._unmarkAction = _unmarkAction.instance( ToolGroupActionInstanceProc, self, unmarkFactory )
		self._regionSelAction = _regionSelAction.instance( ToolGroupActionInstanceProc, self, regionMarkFactory )
		self._growAction = _growAction.instance( ToolGroupActionInstanceProc, self, ProcMeshGrowVertexSelection )
		self._shrinkAction = _shrinkAction.instance( ToolGroupActionInstanceProc, self, ProcMeshShrinkVertexSelection )
		self._moveAction = _moveAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMoveVertices )
		self._scaleAction = _scaleAction.instance( ToolGroupActionInstanceProc, self, ProcMeshScaleVertices )
		self._rotateAction = _rotateAction.instance( ToolGroupActionInstanceProc, self, ProcMeshRotateVertices )
		self._moveNmlAction = _moveNmlAction.instance( ToolGroupActionInstanceProc, self, ProcMeshMoveVerticesNormal )
		self._flattenXAction = _flattenXAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenVerticesMode.X ) )
		self._flattenYAction = _flattenYAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenVerticesMode.Y ) )
		self._flattenZAction = _flattenZAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenVerticesMode.Z ) )
		self._flattenNmlAction = _flattenNmlAction.instance( ToolGroupActionInstanceProc, self, flattenFactory( MeshFlattenVerticesMode.NORMAL ) )
		self._flattenMenuAction = _flattenMenuAction.instance( ToolGroupActionInstanceSubmenu, self )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenXAction.contextMenuItem( self._flattenMenuAction.menu )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenYAction.contextMenuItem( self._flattenMenuAction.menu )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenZAction.contextMenuItem( self._flattenMenuAction.menu )
		self._flattenMenuAction.menu.layout.row()  <<  self._flattenNmlAction.contextMenuItem( self._flattenMenuAction.menu )
		self._tweakAction = _tweakAction.instance( ToolGroupActionInstanceProc, self, ProcMeshTweakComponents, toolInitialiser=tweakToolInitialiser )
		self._surfaceTweakAction = _surfaceTweakAction.instance( ToolGroupActionInstanceProc, self, ProcMeshSurfaceTweakComponents, toolInitialiser=surfaceTweakToolInitialiser )
		self._templateTweakAction = _templateTweakAction.instance( ToolGroupActionInstanceProc, self, ProcMeshTemplateTweakVertices )
		self._proportionalTweakAction = _proportionalTweakAction.instance( ToolGroupActionInstanceProc, self, ProcMeshProportionalTweak )
		self._expandAction = _expandAction.instance( ToolGroupActionInstanceProc, self, ProcMeshExpandVertices )
		self._bevelAction = _bevelAction.instance( ToolGroupActionInstanceProc, self, ProcMeshBevelVertices )
		self._connectAction = _connectAction.instance( ToolGroupActionInstanceProc, self, ProcMeshConnectVertices )
		self._cutAction = _cutAction.instance( ToolGroupActionInstanceProc, self, ProcMeshCut )
		self._nCutAction = _nCutAction.instance( ToolGroupActionInstanceProc, self, ProcMeshNCut )
		self._weldAction = _weldAction.instance( ToolGroupActionInstanceProc, self, ProcMeshWeldVertices )
		self._targetWeldAction = _targetWeldAction.instance( ToolGroupActionInstanceProc, self, ProcMeshTargetWeldVertices )
		self._dissolveAction = _dissolveAction.instance( ToolGroupActionInstanceProc, self, ProcMeshDissolveVertices )
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
											self._markAction.contextMenuItem( self._markMenuAction.menu )
		self._markMenuAction.menu.layout.row()  <<  self._regionSelAction.contextMenuItem( self._markMenuAction.menu )  <<		\
											self._unmarkAction.contextMenuItem( self._markMenuAction.menu )
		self._markMenuAction.menu.layout.row()  <<  self._growAction.contextMenuItem( self._markMenuAction.menu )  <<		\
											self._shrinkAction.contextMenuItem( self._markMenuAction.menu )

		self._moveMenuAction.menu.layout.row()  <<  self._moveAction.contextMenuItem( self._moveMenuAction.menu )  <<	\
											self._scaleAction.contextMenuItem( self._moveMenuAction.menu )
		self._moveMenuAction.menu.layout.row()  <<  self._rotateAction.contextMenuItem( self._moveMenuAction.menu )  <<	\
											self._moveNmlAction.contextMenuItem( self._moveMenuAction.menu )
		self._moveMenuAction.menu.layout.row()  <<  self._flattenMenuAction.contextMenuItem( self._moveMenuAction.menu )  <<	\
											self._tweakAction.contextMenuItem( self._moveMenuAction.menu )
		self._moveMenuAction.menu.layout.row()  <<  self._surfaceTweakAction.contextMenuItem( self._moveMenuAction.menu )  <<  \
											self._templateTweakAction.contextMenuItem( self._moveMenuAction.menu )
		self._moveMenuAction.menu.layout.row()  <<  self._proportionalTweakAction.contextMenuItem( self._moveMenuAction.menu )  <<  None

		self._expansionMenuAction.menu.layout.row()  <<  self._expandAction.contextMenuItem( self._expansionMenuAction.menu )  <<	\
												self._bevelAction.contextMenuItem( self._expansionMenuAction.menu )

		self._cuttingMenuAction.menu.layout.row()  <<  self._connectAction.contextMenuItem( self._cuttingMenuAction.menu )  <<		\
											self._cutAction.contextMenuItem( self._cuttingMenuAction.menu )
		self._cuttingMenuAction.menu.layout.row()  <<  self._nCutAction.contextMenuItem( self._cuttingMenuAction.menu )  <<  None

		self._joiningMenuAction.menu.layout.row()  <<  self._weldAction.contextMenuItem( self._joiningMenuAction.menu )  <<	\
											self._targetWeldAction.contextMenuItem( self._joiningMenuAction.menu )
		self._joiningMenuAction.menu.layout.row()  <<  self._dissolveAction.contextMenuItem( self._joiningMenuAction.menu )  <<  None

		self._creaseMenuAction.menu.layout.row()  <<  self._creaseAction.contextMenuItem( self._creaseMenuAction.menu )  <<  None
		self._creaseMenuAction.menu.layout.row()  <<  self._nsharpAction.contextMenuItem( self._creaseMenuAction.menu )  <<		\
												self._nsmoothAction.contextMenuItem( self._creaseMenuAction.menu )


	def layoutContents(self, layout):
		layout.row()  <<  _( 'Select' )
		layout.row()  <<  self._pickAction.button()  <<  self._regionSelAction.button()  <<  self._markAction.button()
		layout.row()  <<  self._growAction.button()  <<  self._shrinkAction.button()  <<  self._unmarkAction.button()
		layout.row()
		layout.row()  <<  _( 'Move / adjust' )
		layout.row()  <<  self._moveAction.button()  <<  self._scaleAction.button()  <<  self._rotateAction.button()
		layout.row()  <<  self._moveNmlAction.button()  <<  self._flattenMenuAction.button()  <<  None
		layout.row()  <<  self._tweakAction.button()  <<  self._surfaceTweakAction.button()  <<  self._templateTweakAction.button()
		layout.row()  <<  self._proportionalTweakAction.button()  <<  None  <<  None
		layout.row()
		layout.row()  <<  _( 'Expansion' )
		layout.row()  <<  self._expandAction.button()  <<  self._bevelAction.button()  <<  None
		layout.row()
		layout.row()  <<  _( 'Cutting' )
		layout.row()  <<  self._connectAction.button()  <<  self._cutAction.button()  <<  self._nCutAction.button()
		layout.row()
		layout.row()  <<  _( 'Joining' )
		layout.row()  <<  self._weldAction.button()  <<  self._targetWeldAction.button()  <<  self._dissolveAction.button()
		layout.row()
		layout.row()  <<  _( 'Crease' )
		layout.row()  <<  self._creaseAction.button()  <<  self._nsharpAction.button()  <<  self._nsmoothAction.button()
		layout.row()


	def initContextMenu(self, contextMenu):
		contextMenu.layout.row()  <<  self._markMenuAction.contextMenuItem( contextMenu )  <<  self._moveMenuAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._expansionMenuAction.contextMenuItem( contextMenu )  <<  self._cuttingMenuAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._joiningMenuAction.contextMenuItem( contextMenu )  <<  self._creaseMenuAction.contextMenuItem( contextMenu )


