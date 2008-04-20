##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.UIAction import UIAction

from Britefury.Tool.Toolkit import Toolkit

from Britefury.MeshTools.MeshEditVertexToolset import MeshEditVertexToolset
from Britefury.MeshTools.MeshEditEdgeToolset import MeshEditEdgeToolset
from Britefury.MeshTools.MeshEditFaceToolset import MeshEditFaceToolset
from Britefury.MeshTools.MeshEditBodyToolset import MeshEditBodyToolset



_vertexAction = UIAction( 'Toolsets.Mesh.Edit.VertexTools', ( _( 'Toolsets' ), _( 'Mesh' ), _( 'Edit' ), _( 'Vertex tools' ) ), accelerator='<alt>v', tooltip=_( 'Vertex tools' ) )
_edgeAction = UIAction( 'Toolsets.Mesh.Edit.EdgeTools', ( _( 'Toolsets' ), _( 'Mesh' ), _( 'Edit' ), _( 'Edge tools' ) ), accelerator='<alt>e', tooltip=_( 'Edge tools' ) )
_faceAction = UIAction( 'Toolsets.Mesh.Edit.FaceTools', ( _( 'Toolsets' ), _( 'Mesh' ), _( 'Edit' ), _( 'Face tools' ) ), accelerator='<alt>f', tooltip=_( 'Face tools' ) )
_bodyAction = UIAction( 'Toolsets.Mesh.Edit.BodyTools', ( _( 'Toolsets' ), _( 'Mesh' ), _( 'Edit' ), _( 'Body tools' ) ), accelerator='<alt>b', tooltip=_( 'Body tools' ) )


class MeshEditToolkit (Toolkit):
	def __init__(self, nodeEditor, sceneEditorTools, backgroundLayerPainter, view, parentWindow, toolPanel, commandHistory):
		super( MeshEditToolkit, self ).__init__()
		vertexToolset = MeshEditVertexToolset( nodeEditor )
		self.addToolset( vertexToolset, 'meshedit_vertices.png', _vertexAction )
		edgeToolset = MeshEditEdgeToolset( nodeEditor )
		self.addToolset( edgeToolset, 'meshedit_edges.png', _edgeAction )
		faceToolset = MeshEditFaceToolset( nodeEditor, sceneEditorTools, parentWindow, commandHistory )
		self.addToolset( faceToolset, 'meshedit_faces.png', _faceAction )
		bodyToolset = MeshEditBodyToolset( nodeEditor, sceneEditorTools, backgroundLayerPainter, view, parentWindow, toolPanel, commandHistory )
		self.addToolset( bodyToolset, 'meshedit_body.png', _bodyAction )
