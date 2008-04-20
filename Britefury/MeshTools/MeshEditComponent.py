##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO.IOXml import IOXmlEnum

from Britefury.Kernel.Enum import Enum

from Britefury.Cell.Cell import *

from Britefury.Sheet.Sheet import *


class MeshEditComponent (Enum):
	VERTICES = 0
	EDGES = 1
	FACES = 2



targetIOXmlEnum = IOXmlEnum( MeshEditComponent, 'MeshEditComponent', MeshEditComponent.VERTICES )
targetIOXmlEnum.value( MeshEditComponent.VERTICES, 'vertices' )
targetIOXmlEnum.value( MeshEditComponent.EDGES, 'edges' )
targetIOXmlEnum.value( MeshEditComponent.FACES, 'faces' )

ioXmlReadMeshEditComponentProp = targetIOXmlEnum.propReader
ioXmlReadMeshEditComponentNode = targetIOXmlEnum.nodeReader
ioXmlWriteMeshEditComponentProp = targetIOXmlEnum.propWriter
ioXmlWriteMeshEditComponentNode = targetIOXmlEnum.nodeWriter



class MeshEditComponentCell (Cell):
	valueClass = MeshEditComponent
	bAllowNone = False

registerCellType( MeshEditComponent, MeshEditComponentCell )


