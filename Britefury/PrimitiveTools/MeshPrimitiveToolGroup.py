##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.UIAction import UIAction

from Britefury.ProceduralTool.ProcToolGroup import ProcToolGroup, ToolGroupActionInstanceProc, ToolGroupActionInstanceSubmenu

from Britefury.PrimitiveTools.ToolBoxPrimitive import ProcBoxPrimitive
from Britefury.PrimitiveTools.ToolPlanePrimitive import ProcPlanePrimitive
from Britefury.PrimitiveTools.ToolSpherePrimitive import ProcSpherePrimitive
from Britefury.PrimitiveTools.ToolCylinderPrimitive import ProcCylinderPrimitive
from Britefury.PrimitiveTools.ToolConePrimitive import ProcConePrimitive
from Britefury.PrimitiveTools.ToolTorusPrimitive import ProcTorusPrimitive
from Britefury.PrimitiveTools.ToolTubePrimitive import ProcTubePrimitive
from Britefury.MeshTools.MeshEdit.ToolMeshDrawFace import ProcMeshDrawFace
from Britefury.MeshTools.MeshEdit.ToolMeshDrawQuads import ProcMeshDrawQuads




_boxAction = UIAction( 'Tools.Primitives.Mesh.Box', ( _( 'Tools' ), _( 'Primitives' ), _( 'Mesh' ), _( 'Box' ) ),
			_( 'Box' ), _( 'Box' ), 'b', _( 'Box mesh' ) )
_planeAction = UIAction( 'Tools.Primitives.Mesh.Plane', ( _( 'Tools' ), _( 'Primitives' ), _( 'Mesh' ), _( 'Plane' ) ),
			_( 'Plane' ), _( 'Plane' ), 'p', _( 'Plane mesh' ) )
_sphereAction = UIAction( 'Tools.Primitives.Mesh.Sphere', ( _( 'Tools' ), _( 'Primitives' ), _( 'Mesh' ), _( 'Sphere' ) ),
			_( 'Sphere' ), _( 'Sphere' ), 's', _( 'Sphere mesh' ) )
_cylinderAction = UIAction( 'Tools.Primitives.Mesh.Cylinder', ( _( 'Tools' ), _( 'Primitives' ), _( 'Mesh' ), _( 'Cylinder' ) ),
			_( 'Cylinder' ), _( 'Cylinder' ), 'c', _( 'Cylinder mesh' ) )
_coneAction = UIAction( 'Tools.Primitives.Mesh.Cone', ( _( 'Tools' ), _( 'Primitives' ), _( 'Mesh' ), _( 'Cone' ) ),
			_( 'Cone' ), _( 'Cone' ), '<shift>c', _( 'Cone mesh' ) )
_torusAction = UIAction( 'Tools.Primitives.Mesh.Torus', ( _( 'Tools' ), _( 'Primitives' ), _( 'Mesh' ), _( 'Torus' ) ),
			_( 'Torus' ), _( 'Torus' ), 't', _( 'Torus mesh' ) )
_tubeAction = UIAction( 'Tools.Primitives.Mesh.Tube', ( _( 'Tools' ), _( 'Primitives' ), _( 'Mesh' ), _( 'Tube' ) ),
			_( 'Tube' ), _( 'Tube' ), '<shift>t', _( 'Tube mesh' ) )
_drawFaceAction = UIAction( 'Tools.Primitives.Mesh.DrawFace', ( _( 'Tools' ), _( 'Primitives' ), _( 'Mesh' ), _( 'Draw face' ) ),
			_( 'Draw face' ), _( 'Draw face' ), 'f', _( 'Draw face' ) )
_drawQuadsAction = UIAction( 'Tools.Primitives.Mesh.DrawQuadMesh', ( _( 'Tools' ), _( 'Primitives' ), _( 'Mesh' ), _( 'Draw quad mesh' ) ),
			_( 'Draw quads' ), _( 'Draw quad mesh' ), 'm', _( 'Draw quad mesh' ) )



class MeshPrimitiveToolGroup (ProcToolGroup):
	def __init__(self, nodeEditor):
		super( MeshPrimitiveToolGroup, self ).__init__( nodeEditor )
		self._boxAction = _boxAction.instance( ToolGroupActionInstanceProc, self, ProcBoxPrimitive )
		self._planeAction = _planeAction.instance( ToolGroupActionInstanceProc, self, ProcPlanePrimitive )
		self._sphereAction = _sphereAction.instance( ToolGroupActionInstanceProc, self, ProcSpherePrimitive )
		self._cylinderAction = _cylinderAction.instance( ToolGroupActionInstanceProc, self, ProcCylinderPrimitive )
		self._coneAction = _coneAction.instance( ToolGroupActionInstanceProc, self, ProcConePrimitive )
		self._torusAction = _torusAction.instance( ToolGroupActionInstanceProc, self, ProcTorusPrimitive )
		self._tubeAction = _tubeAction.instance( ToolGroupActionInstanceProc, self, ProcTubePrimitive )
		self._tubeAction = _tubeAction.instance( ToolGroupActionInstanceProc, self, ProcTubePrimitive )
		self._drawFaceAction = _drawFaceAction.instance( ToolGroupActionInstanceProc, self, ProcMeshDrawFace )
		self._drawQuadsAction = _drawQuadsAction.instance( ToolGroupActionInstanceProc, self, ProcMeshDrawQuads )


	def layoutContents(self, layout):
		layout.row()  <<  self._boxAction.button()  <<  self._planeAction.button()  <<  self._sphereAction.button()
		layout.row()  <<  self._cylinderAction.button()  <<  self._coneAction.button()  <<  self._torusAction.button()
		layout.row()  <<  self._tubeAction.button()  <<  None  <<  None
		layout.row()  <<  self._drawFaceAction.button()  <<  self._drawQuadsAction.button()  <<  None


	def initContextMenu(self, contextMenu):
		contextMenu.layout.row()  <<  self._boxAction.contextMenuItem( contextMenu )  <<  self._planeAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._sphereAction.contextMenuItem( contextMenu )  <<  self._cylinderAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._coneAction.contextMenuItem( contextMenu )  <<  self._torusAction.contextMenuItem( contextMenu )
		contextMenu.layout.row()  <<  self._tubeAction.contextMenuItem( contextMenu )  <<  None
		contextMenu.layout.row()  <<  self._drawFaceAction.contextMenuItem( contextMenu )  <<  self._drawQuadsAction.contextMenuItem( contextMenu )


