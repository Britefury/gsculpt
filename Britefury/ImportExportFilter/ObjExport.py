##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Mesh.Mesh import GSProductMesh





def _writeProduct(outFile, product, name, bTriangulate, totalVertices, totalVertexTextures, totalVertexNormals, bNegativeIndices):
	"""Private - write a product to a file in OBJ format"""
	if isinstance( product, GSProductMesh ):
		name = name.replace( '"', '\\"' )
		outFile.write( 'g "%s"\n' % ( name, ) )

		vertexTextureIndexOffsets = []
		vertexNormalIndexOffsets = []
		numVertices = product.getNumVertices()
		numVertexTextures = 0
		numVertexNormals = 0
		for vertexIndex in xrange( 0, product.getNumVertices() ):
			p = product.getVertexPosition( vertexIndex )
			outFile.write( 'v %f %f %f\n'  %  ( p.x, p.y, p.z ) )
			vertexTextureIndexOffsets.append( numVertexTextures )
			vertexNormalIndexOffsets.append( numVertexNormals )
			numVertexTextures += product.getNumVertexTextures( vertexIndex )
			numVertexNormals += product.getNumVertexNormals( vertexIndex )

		for vertexIndex in xrange( 0, product.getNumVertices() ):
			for textureIndex in xrange( 0, product.getNumVertexTextures( vertexIndex ) ):
				p = product.getVertexTexturePoint( vertexIndex, textureIndex )
				outFile.write( 'vt %f %f\n'  %  ( p.x, p.y ) )

		for vertexIndex in xrange( 0, product.getNumVertices() ):
			for normalIndex in xrange( 0, product.getNumVertexNormals( vertexIndex ) ):
				n = product.getVertexNormal( vertexIndex, normalIndex )
				outFile.write( 'vn %f %f %f\n'  %  ( n.x, n.y, n.z ) )


		def _writeFaceVertex(outFile, faceIndex, faceVertexIndex):
			vertexIndex, textureIndex, normalIndex = product.getFaceVertex( faceIndex, faceVertexIndex )
			textureIndex += vertexTextureIndexOffsets[vertexIndex]
			normalIndex += vertexNormalIndexOffsets[vertexIndex]
			if bNegativeIndices:
				outFile.write( '%d/%d/%d '  %  ( vertexIndex - numVertices, textureIndex - numVertexTextures, normalIndex - numVertexNormals ) )
			else:
				outFile.write( '%d/%d/%d '  %  ( 1 + totalVertices + vertexIndex, 1 + totalVertexTextures + textureIndex, 1 + totalVertexNormals + normalIndex ) )


		for faceIndex in xrange( 0, product.getNumFaces() ):
			if bTriangulate:
				indexTris = product.getFaceIndexTriangles( faceIndex )
				for tri in indexTris:
					outFile.write( 'f ' )
					_writeFaceVertex( outFile, faceIndex, tri.a )
					_writeFaceVertex( outFile, faceIndex, tri.b )
					_writeFaceVertex( outFile, faceIndex, tri.c )
					outFile.write( '\n' )
			else:
				outFile.write( 'f ' )
				for faceVertexIndex in xrange( 0, product.getFaceSize( faceIndex ) ):
					_writeFaceVertex( outFile, faceIndex, faceVertexIndex )
				outFile.write( '\n' )

		return totalVertices + numVertices, totalVertexTextures + numVertexTextures, totalVertexNormals + numVertexNormals



def exportProduct(f, product, name, bTriangulate, bNegativeIndices):
	"""Export a product in OBJ format to a file
	@f - the file
	@product - the product to export
	@bTriangulate - triangulate all faces if True
	@bNegativeIndices - output negative indices"""
	_writeProduct( f, product, name, bTriangulate, 0, 0, 0, bNegativeIndices )

def exportProducts(f, productsAndNames, bTriangulate, bNegativeIndices):
	"""Export a list of products in OBJ format to a file
	@f - the file
	@productsAndNames - a list of ( product, name ) pairs to export
	@bTriangulate - triangulate all faces if True
	@bNegativeIndices - output negative indices"""
	totalVertices = 0
	totalVertexTextures = 0
	totalVertexNormals = 0

	for product, name in productsAndNames:
		totalVertices, totalVertexTextures, totalVertexNormals = _writeProduct( f, product, name, bTriangulate, totalVertices, totalVertexTextures, totalVertexNormals, bNegativeIndices )
		f.write( '\n\n\n' )



def exportNode(f, node, bTriangulate, bNegativeIndices):
	"""Export the result product of a node, in OBJ format to a file
	@f - the file
	@node - the node whose product is to be exported
	@bTriangulate - triangulate all faces if True
	@bNegativeIndices - output negative indices"""
	_writeProduct( f, node.getResultProduct(), node.name, bTriangulate, 0, 0, 0, bNegativeIndices )

def exportNodes(f, nodes, bTriangulate, bNegativeIndices):
	"""Export the result products of a list of nodes, in OBJ format to a file
	@f - the file
	@nodes - a list of nodes whose products are to be exported
	@bTriangulate - triangulate all faces if True
	@bNegativeIndices - output negative indices"""
	totalVertices = 0
	totalVertexTextures = 0
	totalVertexNormals = 0

	for node in nodes:
		totalVertices, totalVertexTextures, totalVertexNormals = _writeProduct( f, node.getResultProduct(), node.name, bTriangulate, totalVertices, totalVertexTextures, totalVertexNormals, bNegativeIndices )
		f.write( '\n\n\n' )

