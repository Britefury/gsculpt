##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO.IOXml import IOXmlEnum

from Britefury.Cell.Cell import *
from Britefury.Sheet.Sheet import *

from Britefury.CellEdit.ChoiceCellEditRadioGroup import *
from Britefury.SheetEdit.SheetEdit import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.Mesh.Mesh import MeshWeldStyle



# XML IO for MeshWeldStyle

directionIOXmlEnum = IOXmlEnum( MeshWeldStyle, 'MeshWeldStyle', MeshWeldStyle.MIDPOINT )
directionIOXmlEnum.value( MeshWeldStyle.MIDPOINT, 'mid_point' )
directionIOXmlEnum.value( MeshWeldStyle.A, 'a' )
directionIOXmlEnum.value( MeshWeldStyle.B, 'b' )

ioXmlReadMeshWeldStyleProp = directionIOXmlEnum.propReader
ioXmlReadMeshWeldStyleNode = directionIOXmlEnum.nodeReader
ioXmlWriteMeshWeldStyleProp = directionIOXmlEnum.propWriter
ioXmlWriteMeshWeldStyleNode = directionIOXmlEnum.nodeWriter


# Cell for MeshWeldStyle

class MeshWeldStyleCell (Cell):
	valueClass = MeshWeldStyle
	bAllowNone = False

registerCellType( MeshWeldStyle, MeshWeldStyleCell )


# Cell editor for MeshWeldStyle

class MeshWeldStyleCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshWeldStyleCellEditRadioGroup, self ).__init__()
		self.midPoint = self.addChoice( MeshWeldStyle.MIDPOINT, _( 'Mid-point' ) )
		self.a = self.addChoice( MeshWeldStyle.A, 'A' )
		self.b = self.addChoice( MeshWeldStyle.B, 'B' )



# Field editor for MeshWeldStyle

class MeshWeldStyleFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'midPoint', 'a', 'b' ]

	def createElement(self, sheetEditContext):
		return MeshWeldStyleCellEditRadioGroup()

