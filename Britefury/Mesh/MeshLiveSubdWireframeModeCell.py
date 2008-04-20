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

from Britefury.Mesh.Mesh import MeshLiveSubdWireframeMode





# XML IO for MeshLiveSubdWireframeMode

liveSubdWireframeModeIOXmlEnum = IOXmlEnum( MeshLiveSubdWireframeMode, 'MeshLiveSubdWireframeMode', MeshLiveSubdWireframeMode.CONTROL )
liveSubdWireframeModeIOXmlEnum.value( MeshLiveSubdWireframeMode.NONE, 'none' )
liveSubdWireframeModeIOXmlEnum.value( MeshLiveSubdWireframeMode.CONTROL, 'control' )
liveSubdWireframeModeIOXmlEnum.value( MeshLiveSubdWireframeMode.FULL, 'full' )

ioXmlReadMeshLiveSubdWireframeModeProp = liveSubdWireframeModeIOXmlEnum.propReader
ioXmlReadMeshLiveSubdWireframeModeNode = liveSubdWireframeModeIOXmlEnum.nodeReader
ioXmlWriteMeshLiveSubdWireframeModeProp = liveSubdWireframeModeIOXmlEnum.propWriter
ioXmlWriteMeshLiveSubdWireframeModeNode = liveSubdWireframeModeIOXmlEnum.nodeWriter


# Cell for MeshLiveSubdWireframeMode

class MeshLiveSubdWireframeModeCell (Cell):
	valueClass = MeshLiveSubdWireframeMode
	bAllowNone = False

registerCellType( MeshLiveSubdWireframeMode, MeshLiveSubdWireframeModeCell )

