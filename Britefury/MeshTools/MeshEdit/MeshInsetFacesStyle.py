##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO.IOXml import IOXmlEnum

from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.Mesh.Mesh import MeshInsetFacesStyle



# XML IO for MeshInsetFacesStyle

styleIOXmlEnum = IOXmlEnum( MeshInsetFacesStyle, 'MeshInsetFacesStyle', MeshInsetFacesStyle.FROMEDGES )
styleIOXmlEnum.value( MeshInsetFacesStyle.FROMEDGES, 'from_edges' )
styleIOXmlEnum.value( MeshInsetFacesStyle.TOCENTRE, 'to_centre' )

ioXmlReadMeshInsetFacesStyleProp = styleIOXmlEnum.propReader
ioXmlReadMeshInsetFacesStyleNode = styleIOXmlEnum.nodeReader
ioXmlWriteMeshInsetFacesStyleProp = styleIOXmlEnum.propWriter
ioXmlWriteMeshInsetFacesStyleNode = styleIOXmlEnum.nodeWriter


# Cell for MeshInsetFacesStyle

class MeshInsetFacesStyleCell (Cell):
	valueClass = MeshInsetFacesStyle
	bAllowNone = False

registerCellType( MeshInsetFacesStyle, MeshInsetFacesStyleCell )


# Cell editor for MeshInsetFacesStyle

class MeshInsetFacesStyleCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshInsetFacesStyleCellEditRadioGroup, self ).__init__()
		self.fromEdges = self.addChoice( MeshInsetFacesStyle.FROMEDGES, _( 'From edges' ) )
		self.toCentre = self.addChoice( MeshInsetFacesStyle.TOCENTRE, _( 'To centre' ) )



# Field editor for MeshInsetFacesStyle

class MeshInsetFacesStyleFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'fromEdges', 'toCentre' ]

	def createElement(self, rowEditContext):
		return MeshInsetFacesStyleCellEditRadioGroup()

