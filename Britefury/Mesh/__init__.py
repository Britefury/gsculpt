##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO.IOXml import ioObjectFactoryRegister

from Britefury.Mesh.Mesh import GSProductMesh, MeshNCutState, MeshDrawQuadsState


def getVertexPosition(self, index):
	if index < 0  or  index >= self.getNumVertices():
		raise IndexError, 'vertex index out of bounds; %d' % ( index, )
	return self._p_getVertexPosition( index )

def setVertexPosition(self, index, position):
	if index < 0  or  index >= self.getNumVertices():
		raise IndexError, 'vertex index out of bounds; %d' % ( index, )
	return self._p_setVertexPosition( index, position )

def isVertexMarked(self, index):
	if index < 0  or  index >= self.getNumVertices():
		raise IndexError, 'vertex index out of bounds; %d' % ( index, )
	return self._p_isVertexMarked( index )

def getVertexTexturePoint(self, vertexIndex, textureIndex):
	if textureIndex < 0  or  textureIndex >= self.getNumVertexTextures( vertexIndex ):
		raise IndexError, 'texture index out of bounds; %d' % ( textureIndex, )
	return self._p_getVertexTexturePoint( vertexIndex, textureIndex )

def getNumVertexTextures(self, vertexIndex):
	if vertexIndex < 0  or  vertexIndex >= self.getNumVertices():
		raise IndexError, 'vertex index out of bounds; %d' % ( vertexIndex, )
	return self._p_getNumVertexTextures( vertexIndex )

def getVertexNormal(self, vertexIndex, normalIndex):
	if normalIndex < 0  or  normalIndex >= self.getNumVertexNormals( vertexIndex ):
		raise IndexError, 'normal index out of bounds; %d' % ( normalIndex, )
	return self._p_getVertexNormal( vertexIndex, normalIndex )

def getNumVertexNormals(self, vertexIndex):
	if vertexIndex < 0  or  vertexIndex >= self.getNumVertices():
		raise IndexError, 'vertex index out of bounds; %d' % ( vertexIndex, )
	return self._p_getNumVertexNormals( vertexIndex )


def getEdgeSegment(self, index):
	if index < 0  or  index >= self.getNumEdges():
		raise IndexError, 'edge index out of bounds; %d' % ( index, )
	return self._p_getEdgeSegment( index )

def isEdgeMarked(self, index):
	if index < 0  or  index >= self.getNumEdges():
		raise IndexError, 'edge index out of bounds; %d' % ( index, )
	return self._p_isEdgeMarked( index )

def isEdgeBoundary(self, index):
	if index < 0  or  index >= self.getNumEdges():
		raise IndexError, 'edge index out of bounds; %d' % ( index, )
	return self._p_isEdgeBoundary( index )

def getEdgeVertexAIndex(self, edgeIndex):
	if edgeIndex < 0  or  edgeIndex >= self.getNumEdges():
		raise IndexError, 'edge index out of bounds; %d' % ( edgeIndex, )
	return self._p_getEdgeVertexAIndex( edgeIndex )

def getEdgeVertexBIndex(self, edgeIndex):
	if edgeIndex < 0  or  edgeIndex >= self.getNumEdges():
		raise IndexError, 'edge index out of bounds; %d' % ( edgeIndex, )
	return self._p_getEdgeVertexBIndex( edgeIndex )

def getEdgeOppositeVertexIndex(self, edgeIndex, vertexIndex):
	if edgeIndex < 0  or  edgeIndex >= self.getNumEdges():
		raise IndexError, 'edge index out of bounds; %d' % ( edgeIndex, )
	if vertexIndex < 0  or  vertexIndex >= self.getNumVertices():
		raise IndexError, 'vertex index out of bounds; %d' % ( vertexIndex, )
	return self._p_getEdgeOppositeVertexIndex( edgeIndex, vertexIndex )

def getEdgeFaceAIndex(self, edgeIndex):
	if edgeIndex < 0  or  edgeIndex >= self.getNumEdges():
		raise IndexError, 'edge index out of bounds; %d' % ( edgeIndex, )
	return self._p_getEdgeFaceAIndex( edgeIndex )

def getEdgeFaceBIndex(self, edgeIndex):
	if edgeIndex < 0  or  edgeIndex >= self.getNumEdges():
		raise IndexError, 'edge index out of bounds; %d' % ( edgeIndex, )
	return self._p_getEdgeFaceBIndex( edgeIndex )

def getEdgeOppositeFaceIndex(self, edgeIndex, faceIndex):
	if edgeIndex < 0  or  edgeIndex >= self.getNumEdges():
		raise IndexError, 'edge index out of bounds; %d' % ( edgeIndex, )
	if faceIndex < 0  or  faceIndex >= self.getNumFaces():
		raise IndexError, 'face index out of bounds; %d' % ( faceIndex, )
	return self._p_getEdgeOppositeFaceIndex( edgeIndex, faceIndex )


def getFaceSize(self, faceIndex):
	if faceIndex < 0  or  faceIndex >= self.getNumFaces():
		raise IndexError, 'face index out of bounds; %d' % ( faceIndex, )
	return self._p_getFaceSize( faceIndex )

def getFaceVertex(self, faceIndex, faceVertexIndex):
	if faceVertexIndex < 0  or  faceVertexIndex >= self.getFaceSize( faceIndex ):
		raise IndexError, 'face vertex index out of bounds; %d' % ( faceVertexIndex, )
	return self._p_getFaceVertex( faceIndex, faceVertexIndex )

def getFaceIndexTriangles(self, index):
	if index < 0  or  index >= self.getNumFaces():
		raise IndexError, 'face index out of bounds; %d' % ( index, )
	return self._p_getFaceIndexTriangles( index )

def getFaceTriangles(self, index):
	if index < 0  or  index >= self.getNumFaces():
		raise IndexError, 'face index out of bounds; %d' % ( index, )
	return self._p_getFaceTriangles( index )

def getFaceCentroid(self, index):
	if index < 0  or  index >= self.getNumFaces():
		raise IndexError, 'face index out of bounds; %d' % ( index, )
	return self._p_getFaceCentroid( index )

def isFaceMarked(self, index):
	if index < 0  or  index >= self.getNumFaces():
		raise IndexError, 'face index out of bounds; %d' % ( index, )
	return self._p_isFaceMarked( index )



def getMarkedVerticesBBox(self):
	if self.getNumMarkedVertices() > 0:
		return self._p_getMarkedVerticesBBox()
	else:
		return None

def getMarkedVerticesCentre(self):
	if self.getNumMarkedVertices() > 0:
		return self._p_getMarkedVerticesCentre()
	else:
		return None


def getMarkedEdgesBBox(self):
	if self.getNumMarkedEdges() > 0:
		return self._p_getMarkedEdgesBBox()
	else:
		return None

def getMarkedEdgesCentre(self):
	if self.getNumMarkedEdges() > 0:
		return self._p_getMarkedEdgesCentre()
	else:
		return None


def getMarkedFacesBBox(self):
	if self.getNumMarkedFaces() > 0:
		return self._p_getMarkedFacesBBox()
	else:
		return None

def getMarkedFacesCentre(self):
	if self.getNumMarkedFaces() > 0:
		return self._p_getMarkedFacesCentre()
	else:
		return None






def isValidWeldTargetVertex(self, sourceVertexIndex, targetVertexIndex):
	if sourceVertexIndex < 0  or  sourceVertexIndex >= self.getNumVertices():
		raise IndexError, 'source vertex index out of bounds; %d' % ( sourceVertexIndex, )
	if targetVertexIndex < 0  or  targetVertexIndex >= self.getNumVertices():
		raise IndexError, 'target vertex index out of bounds; %d' % ( targetVertexIndex, )
	return self._p_isValidWeldTargetVertex( sourceVertexIndex, targetVertexIndex )

def isValidWeldTargetEdge(self, sourceVertexIndex, targetEdgeIndex, targetType):
	if sourceVertexIndex < 0  or  sourceVertexIndex >= self.getNumVertices():
		raise IndexError, 'source vertex index out of bounds; %d' % ( sourceVertexIndex, )
	if targetEdgeIndex < 0  or  targetEdgeIndex >= self.getNumEdges():
		raise IndexError, 'target edge index out of bounds; %d' % ( targetEdgeIndex, )
	return self._p_isValidWeldTargetEdge( sourceVertexIndex, targetEdgeIndex, targetType )

def isValidRewireEdgeTarget(self, edgeToRewireIndex, sourceVertexIndex, targetVertexIndex):
	if edgeToRewireIndex < 0  or  edgeToRewireIndex >= self.getNumEdges():
		raise IndexError, 'edge to rewire index out of bounds; %d' % ( edgeToRewireIndex, )
	if sourceVertexIndex < 0  or  sourceVertexIndex >= self.getNumVertices():
		raise IndexError, 'source vertex index out of bounds; %d' % ( sourceVertexIndex, )
	if targetVertexIndex < 0  or  targetVertexIndex >= self.getNumVertices():
		raise IndexError, 'target vertex index out of bounds; %d' % ( targetVertexIndex, )
	return self._p_isValidRewireEdgeTarget( edgeToRewireIndex, sourceVertexIndex, targetVertexIndex )

def canEdgeBeRewired(self, edgeToRewireIndex):
	if edgeToRewireIndex < 0  or  edgeToRewireIndex >= self.getNumEdges():
		raise IndexError, 'edge to rewire index out of bounds; %d' % ( edgeToRewireIndex, )
	return self._p_canEdgeBeRewired( edgeToRewireIndex )

def bandsawGetRingSegments(self, seedEdgeIndex, seedFaceIndex, bStopAtMarkedEdges):
	if seedEdgeIndex < 0  or  seedEdgeIndex >= self.getNumEdges():
		raise IndexError, 'seed edge index out of bounds; %d' % ( seedEdgeIndex, )
	if seedFaceIndex < 0  or  seedFaceIndex >= self.getNumFaces():
		raise IndexError, 'seed face index out of bounds; %d' % ( seedFaceIndex, )
	return self._p_bandsawGetRingSegments( seedEdgeIndex, seedFaceIndex, bStopAtMarkedEdges )

def connectMarkedEdgesGetRingSegments(self, seedEdgeIndex):
	if seedEdgeIndex < 0  or  seedEdgeIndex >= self.getNumEdges():
		raise IndexError, 'seed edge index out of bounds; %d' % ( seedEdgeIndex, )
	return self._p_connectMarkedEdgesGetRingSegments( seedEdgeIndex )






GSProductMesh.getVertexPosition = getVertexPosition
GSProductMesh.setVertexPosition = setVertexPosition
GSProductMesh.isVertexMarked = isVertexMarked
GSProductMesh.getVertexTexturePoint = getVertexTexturePoint
GSProductMesh.getNumVertexTextures = getNumVertexTextures
GSProductMesh.getVertexNormal = getVertexNormal
GSProductMesh.getNumVertexNormals = getNumVertexNormals

GSProductMesh.getEdgeSegment = getEdgeSegment
GSProductMesh.isEdgeMarked = isEdgeMarked
GSProductMesh.isEdgeBoundary = isEdgeBoundary
GSProductMesh.isEdgeBoundary = isEdgeBoundary
GSProductMesh.getEdgeOppositeVertexIndex = getEdgeOppositeVertexIndex

GSProductMesh.getFaceSize = getFaceSize
GSProductMesh.getFaceVertex = getFaceVertex
GSProductMesh.getFaceIndexTriangles = getFaceIndexTriangles
GSProductMesh.getFaceTriangles = getFaceTriangles
GSProductMesh.getFaceCentroid = getFaceCentroid
GSProductMesh.isFaceMarked = isFaceMarked

GSProductMesh.getMarkedVerticesBBox = getMarkedVerticesBBox
GSProductMesh.getMarkedVerticesCentre = getMarkedVerticesCentre
GSProductMesh.getMarkedEdgesBBox = getMarkedEdgesBBox
GSProductMesh.getMarkedEdgesCentre = getMarkedEdgesCentre
GSProductMesh.getMarkedFacesBBox = getMarkedFacesBBox
GSProductMesh.getMarkedFacesCentre = getMarkedFacesCentre

GSProductMesh.isValidWeldTargetVertex = isValidWeldTargetVertex
GSProductMesh.isValidWeldTargetEdge = isValidWeldTargetEdge

GSProductMesh.isValidRewireEdgeTarget = isValidRewireEdgeTarget
GSProductMesh.canEdgeBeRewired = canEdgeBeRewired

GSProductMesh.connectMarkedEdgesGetRingSegments = connectMarkedEdgesGetRingSegments

GSProductMesh.bandsawGetRingSegments = bandsawGetRingSegments


ioObjectFactoryRegister( 'GSProductMesh', GSProductMesh )



def lastSplitVertexIndices(self):
	for i in xrange( 0, len( self ) ):
		yield self._p_getLastSplitVertexIndex( i )

def lastSplitVertexPositions(self):
	for i in xrange( 0, len( self ) ):
		yield self._p_getLastSplitVertexPosition( i )


MeshNCutState.lastSplitVertexIndices = lastSplitVertexIndices
MeshNCutState.lastSplitVertexPositions = lastSplitVertexPositions




def getQuadMeshVertexPosition(self, column, row):
	if column < 0  or  column >= self.getNumColumns():
		raise IndexError, '@column out of bounds'
	if row < 0  or  row >= self.getNumRows():
		raise IndexError, '@row out of bounds'
	bSuccess, pos = self._p_getQuadMeshVertexPosition( column, row )
	if bSuccess:
		return pos
	else:
		return None

MeshDrawQuadsState.getQuadMeshVertexPosition = getQuadMeshVertexPosition

