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

from Britefury.Mesh.Mesh import MeshPreserveNormalSharpness





# XML IO for MeshPreserveNormalSharpness

preserveSharpnessIOXmlEnum = IOXmlEnum( MeshPreserveNormalSharpness, 'MeshPreserveNormalSharpness', MeshPreserveNormalSharpness.IF_SHARP )
preserveSharpnessIOXmlEnum.value( MeshPreserveNormalSharpness.PRESERVE, 'preserve' )
preserveSharpnessIOXmlEnum.value( MeshPreserveNormalSharpness.IF_SHARP, 'if_sharp' )
preserveSharpnessIOXmlEnum.value( MeshPreserveNormalSharpness.SMOOTH, 'smooth' )

ioXmlReadMeshPreserveNormalSharpnessProp = preserveSharpnessIOXmlEnum.propReader
ioXmlReadMeshPreserveNormalSharpnessNode = preserveSharpnessIOXmlEnum.nodeReader
ioXmlWriteMeshPreserveNormalSharpnessProp = preserveSharpnessIOXmlEnum.propWriter
ioXmlWriteMeshPreserveNormalSharpnessNode = preserveSharpnessIOXmlEnum.nodeWriter


# Cell for MeshPreserveNormalSharpness

class MeshPreserveNormalSharpnessCell (Cell):
	valueClass = MeshPreserveNormalSharpness
	bAllowNone = False

registerCellType( MeshPreserveNormalSharpness, MeshPreserveNormalSharpnessCell )

