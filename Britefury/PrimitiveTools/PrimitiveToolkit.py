##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.UIAction import UIAction

from Britefury.Tool.Toolkit import Toolkit

from Britefury.PrimitiveTools.MeshPrimitiveToolset import MeshPrimitiveToolset



_meshAction = UIAction( 'Toolsets.Primitives.Mesh', ( _( 'Toolsets' ), _( 'Primitives' ), _( 'Mesh primitive tools' ) ), accelerator='<alt>m', tooltip=_( 'Mesh primtive tools' ) )



class PrimitiveToolkit (Toolkit):
	def __init__(self, nodeEditor):
		super( PrimitiveToolkit, self ).__init__()
		meshToolset = MeshPrimitiveToolset( nodeEditor )
		self.addToolset( meshToolset, 'mesh_primitives.png', _meshAction )
