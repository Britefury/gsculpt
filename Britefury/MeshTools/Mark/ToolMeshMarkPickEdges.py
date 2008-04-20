##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Kernel.Enum import *

from Britefury.Sheet.Sheet import *

from Britefury.Model import ModelDraw
from Britefury.Model.Pick3d import Pick3d
from Britefury.Model.TargetList import TargetList
from Britefury.Model.Model import pyMPickList_to_MPickList, MPickList

from Britefury.MeshTools.Mark.ToolMeshMarkPick import *




# XML IO for MeshPickEdgesTargetType

class MeshPickEdgesTargetType (Enum):
	EDGE = 0
	LOOP = 1
	RING = 2
	BOUNDARY = 3


pickEdgesTargetTypeIOXmlEnum = IOXmlEnum( MeshPickEdgesTargetType, 'MeshPickEdgesTargetType', MeshPickEdgesTargetType.EDGE )
pickEdgesTargetTypeIOXmlEnum.value( MeshPickEdgesTargetType.EDGE, 'edge' )
pickEdgesTargetTypeIOXmlEnum.value( MeshPickEdgesTargetType.LOOP, 'loop' )
pickEdgesTargetTypeIOXmlEnum.value( MeshPickEdgesTargetType.RING, 'ring' )
pickEdgesTargetTypeIOXmlEnum.value( MeshPickEdgesTargetType.BOUNDARY, 'boundary' )

ioXmlReadMeshPickEdgesTargetTypeProp = pickEdgesTargetTypeIOXmlEnum.propReader
ioXmlReadMeshPickEdgesTargetTypeNode = pickEdgesTargetTypeIOXmlEnum.nodeReader
ioXmlWriteMeshPickEdgesTargetTypeProp = pickEdgesTargetTypeIOXmlEnum.propWriter
ioXmlWriteMeshPickEdgesTargetTypeNode = pickEdgesTargetTypeIOXmlEnum.nodeWriter





class MeshPickEdgesTarget (object):
	def __init__(self, pick=None, targetType=MeshPickEdgesTargetType.EDGE):
		if pick is None:
			pick = Pick3d()
		self.pick = pick
		self.targetType = targetType


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'pick' )  >>  self.pick
		self.targetType = ioXmlReadMeshPickEdgesTargetTypeNode( xmlNode.getChild( 'target_type' ), self.targetType )

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'pick' )  <<  self.pick
		ioXmlWriteMeshPickEdgesTargetTypeNode( xmlNode.addChild( 'target_type' ), self.targetType )


ioObjectFactoryRegister( 'MeshPickEdgesTarget', MeshPickEdgesTarget )




# Each task is of the form:
# ( TargetType, [ Pick ] )
def _meshPickEdgesTargetListToPickTaskList(targets):
	taskList = []

	targetType = None
	taskPicks = None

	for target in targets:
		if isinstance( target, MeshPickEdgesTarget ):
			t = target.targetType
			p = target.pick
		else:
			t = MeshPickEdgesTargetType.EDGE
			p = target

		if t != targetType:
			targetType = t
			taskPicks = []
			task = targetType, taskPicks
			taskList.append( task )

		taskPicks.append( p )

	return taskList




def meshPickEdgesGetTargetType(viewport):
	pstat = viewport.pointerStatus
	if not pstat.bShiftKey and not pstat.bControlKey:
		return MeshPickEdgesTargetType.EDGE
	elif pstat.bShiftKey and not pstat.bControlKey:
		return MeshPickEdgesTargetType.LOOP
	elif not pstat.bShiftKey and pstat.bControlKey:
		return MeshPickEdgesTargetType.RING
	elif pstat.bShiftKey and pstat.bControlKey:
		return MeshPickEdgesTargetType.BOUNDARY
	else:
		return MeshPickEdgesTargetType.EDGE


def meshPickEdgesDiscoverEdgeIndexList(mesh, seedEdgeIndex, targetType):
	if targetType == MeshPickEdgesTargetType.EDGE:
		return [ seedEdgeIndex ]
	elif targetType == MeshPickEdgesTargetType.LOOP:
		return mesh.discoverEdgeLoop( seedEdgeIndex )
	elif targetType == MeshPickEdgesTargetType.RING:
		return mesh.discoverEdgeRing( seedEdgeIndex )
	elif targetType == MeshPickEdgesTargetType.BOUNDARY:
		if mesh.isEdgeBoundary( seedEdgeIndex ):
			return mesh.discoverEdgeBoundary( seedEdgeIndex )
		else:
			return None
	else:
		raise TypeError, 'invalid target type'






class ProcMeshMarkPickEdges (ProcMeshMarkPick):
	description = _( 'Pick edges' )

	def procedureInvoke(self, inputObject, errorHandler):
		taskList =_meshPickEdgesTargetListToPickTaskList( self.targetList )

		for targetType, picks in taskList:
			mpicks = MPickList()
			pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in picks ], mpicks )

			if targetType == MeshPickEdgesTargetType.EDGE:
				inputObject.markEdges_pick( mpicks )
			elif targetType == MeshPickEdgesTargetType.LOOP:
				inputObject.markEdgeLoops_pick( mpicks )
			elif targetType == MeshPickEdgesTargetType.RING:
				inputObject.markEdgeRings_pick( mpicks )
			elif targetType == MeshPickEdgesTargetType.BOUNDARY:
				inputObject.markEdgeBoundaries_pick( mpicks )
			else:
				raise TypeError, 'invalid target type'

		return inputObject





class MeshMarkPickEdgesManipulator (MeshMarkPickEdgeListManipulator):
	def _p_getTargetType(self, viewport):
		return meshPickEdgesGetTargetType( viewport )


	def _p_discoverEdgeList(self, mesh, seedEdgeIndex, targetType):
		discoveredEdgeIndices = meshPickEdgesDiscoverEdgeIndexList( mesh, seedEdgeIndex, targetType )
		if discoveredEdgeIndices is not None:
			discoveredSegments = self._o_edgeIndexListToSegmentList( mesh, discoveredEdgeIndices )
			return True, discoveredEdgeIndices, discoveredSegments
		else:
			return False, None, None


	def _p_pickConfirmed(self, viewport, pick):
		if self._targetType == MeshPickEdgesTargetType.EDGE:
			self._p_commitPick( viewport, pick )
		else:
			self._p_commitPick( viewport, MeshPickEdgesTarget( copy( pick ), self._targetType ) )



class MeshMarkPickEdgesFieldManipulator (MeshMarkPickFieldManipulator):
	def createElement(self, rowEditContext):
		return MeshMarkPickEdgesManipulator()







class MeshMarkPickEdgesAdjustableInvoker (MeshMarkPickAdjustableInvoker):
	functionClass = ProcMeshMarkPickEdges

	def save(self):
		self._inputObject.markEdges_save()

	def restore(self):
		self._inputObject.markEdges_restore()

	def adjust(self):
		taskList =_meshPickEdgesTargetListToPickTaskList( self._proc.targetList )

		for targetType, picks in taskList:
			mpicks = MPickList()
			pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in picks ], mpicks )

			if targetType == MeshPickEdgesTargetType.EDGE:
				self._inputObject.markEdges_pick( mpicks )
			elif targetType == MeshPickEdgesTargetType.LOOP:
				self._inputObject.markEdgeLoops_pick( mpicks )
			elif targetType == MeshPickEdgesTargetType.RING:
				self._inputObject.markEdgeRings_pick( mpicks )
			elif targetType == MeshPickEdgesTargetType.BOUNDARY:
				self._inputObject.markEdgeBoundaries_pick( mpicks )
			else:
				raise TypeError, 'invalid target type'






class ToolMeshMarkPickEdges (ToolMeshMarkPick):
	sheet = SheetRefField( ProcMeshMarkPickEdges )

	functionClass = ProcMeshMarkPickEdges
	adjustableInvoker = MeshMarkPickEdgesAdjustableInvoker
	pageTitle = _( 'Pick' )
	titleText = _( 'Pick Edges' )

	targetListManip = MeshMarkPickEdgesFieldManipulator( sheet.targetList )
	targetListManip.enable()


	layout = FormLayoutFactory()
	layout.textBlock( _( 'Click on edges to pick them\n'   'Hold <shift> to pick loops.\n'   'Hold <control> to pick rings.\n'   'Hold <shift>+<control> to pick boundaries.' ) )
	layout.row()  <<  ''
	layout.finalise()







def _meshPickListToTargetList(picks, targetType):
	targets = TargetList()
	targets[:] = [ MeshPickEdgesTarget( pick, targetType )   for pick in picks ]
	return targets



class ProcMeshMarkPickEdgeLoops (ProcMeshMarkPick):
	__iotypename__ = 'ProcMeshMarkPickEdgeLoops'

	description = '**DEPRACATED**'

	def __ioreplacement__(self):
		proc = ProcMeshMarkPickEdges()
		proc.targetList = _meshPickListToTargetList( self.targetList, MeshPickEdgesTargetType.LOOP )
		return proc



class ProcMeshMarkPickEdgeRings (ProcMeshMarkPick):
	__iotypename__ = 'ProcMeshMarkPickEdgeRings'

	description = '**DEPRACATED**'

	def __ioreplacement__(self):
		proc = ProcMeshMarkPickEdges()
		proc.targetList = _meshPickListToTargetList( self.targetList, MeshPickEdgesTargetType.RING )
		return proc



class ProcMeshMarkPickEdgeBoundaries (ProcMeshMarkPick):
	__iotypename__ = 'ProcMeshMarkPickEdgeBoundaries'

	description = '**DEPRACATED**'

	def __ioreplacement__(self):
		proc = ProcMeshMarkPickEdges()
		proc.targetList = _meshPickListToTargetList( self.targetList, MeshPickEdgesTargetType.BOUNDARY )
		return proc






