##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegisterPrimitive, ioObjectFactoryRegister

from Britefury.Model.Pick3d import Pick3d

from Britefury.Mesh.Mesh import CppMeshCutTarget, MeshCutTargetType




# XML IO for MeshCutTargetType

cutTargetTypeIOXmlEnum = IOXmlEnum( MeshCutTargetType, 'MeshCutTargetType', MeshCutTargetType.EDGE )
cutTargetTypeIOXmlEnum.value( MeshCutTargetType.VERTEX, 'vertex' )
cutTargetTypeIOXmlEnum.value( MeshCutTargetType.EDGE, 'edge' )
cutTargetTypeIOXmlEnum.value( MeshCutTargetType.FACE, 'face' )

ioXmlReadMeshCutTargetTypeProp = cutTargetTypeIOXmlEnum.propReader
ioXmlReadMeshCutTargetTypeNode = cutTargetTypeIOXmlEnum.nodeReader
ioXmlWriteMeshCutTargetTypeProp = cutTargetTypeIOXmlEnum.propWriter
ioXmlWriteMeshCutTargetTypeNode = cutTargetTypeIOXmlEnum.nodeWriter

ioObjectFactoryRegisterPrimitive( 'MeshCutTargetType', MeshCutTargetType )





class MeshCutTarget (object):
	def __init__(self, pick = None, targetType = MeshCutTargetType.EDGE):
		if pick is None:
			pick = Pick3d()
		self.pick = pick
		self.targetType = targetType


	def toCppMeshCutTarget(self):
		return CppMeshCutTarget( self.pick.toMPick(), self.targetType )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'pick' )  >>  self.pick
		self.targetType = ioXmlReadMeshCutTargetTypeNode( xmlNode.getChild( 'target_type' ), self.targetType )

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'pick' )  <<  self.pick
		ioXmlWriteMeshCutTargetTypeNode( xmlNode.addChild( 'target_type' ), self.targetType )


ioObjectFactoryRegister( 'MeshCutTarget', MeshCutTarget )


