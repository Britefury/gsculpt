##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Joerg Scheurich and Geoffrey French 1999-2007.
##-*************************
from Britefury.Mesh.Mesh import GSProductMesh





def _writeProduct(outFile, product, name, bTriangulate, totalVertices, totalVertexTextures, totalVertexNormals):
	"""Private - write a product to a file in VRML97 format"""
	if isinstance( product, GSProductMesh ):
		name = name.replace( '"', '_' )

		vertexTextureIndexOffsets = []
		vertexNormalIndexOffsets = []
		numVertices = product.getNumVertices()
		numVertexTextures = 0
		numVertexNormals = 0
		outFile.write( '#VRML V2.0 utf8\n')
		outFile.write( '# "%s"\n' % ( name, ) )
		outFile.write( 'Shape {\n appearance Appearance {\n')
		outFile.write( '  material Material {}\n }\n')
		outFile.write( ' geometry DEF %s IndexedFaceSet {\n' % ( name, ))

		outFile.write( '  coord Coordinate {\n')
		outFile.write( '   point [\n')
		for vertexIndex in xrange( 0, product.getNumVertices() ):
			p = product.getVertexPosition( vertexIndex )
			outFile.write( '    %f %f %f\n'  %  ( p.x / 100.0, p.y / 100.0, p.z / 100.0 ) )
			vertexTextureIndexOffsets.append( numVertexTextures )
			vertexNormalIndexOffsets.append( numVertexNormals )
			numVertexTextures += product.getNumVertexTextures( vertexIndex )
			numVertexNormals += product.getNumVertexNormals( vertexIndex )
		outFile.write( '   ]\n')
		outFile.write( '  }\n')

		outFile.write( '  texCoord TextureCoordinate {\n')
		outFile.write( '   point [\n')
		for vertexIndex in xrange( 0, product.getNumVertices() ):
			for textureIndex in xrange( 0, product.getNumVertexTextures( vertexIndex ) ):
				p = product.getVertexTexturePoint( vertexIndex, textureIndex )
				outFile.write( '    %f %f\n'  %  ( p.x, p.y ) )
		outFile.write( '   ]\n')
		outFile.write( '  }\n')


		outFile.write( '#  normal Normal {\n')
		outFile.write( '#   vector [\n')
		for vertexIndex in xrange( 0, product.getNumVertices() ):
			for normalIndex in xrange( 0, product.getNumVertexNormals( vertexIndex ) ):
				n = product.getVertexNormal( vertexIndex, normalIndex )
				outFile.write( '#    %f %f %f\n'  %  ( n.x, n.y, n.z ) )
		outFile.write( '#   ]\n')
		outFile.write( '#  }\n')

		def _writeFaceVertex(outFile, faceIndex, faceVertexIndex):
			vertexIndex, textureIndex, normalIndex = product.getFaceVertex( faceIndex, faceVertexIndex )
			outFile.write( '   %d\n'  %  ( vertexIndex ) )

		def _writeTexFaceVertex(outFile, faceIndex, faceVertexIndex):
			vertexIndex, textureIndex, normalIndex = product.getFaceVertex( faceIndex, faceVertexIndex )
			textureIndex += vertexTextureIndexOffsets[vertexIndex]
			outFile.write( '   %d\n'  %  ( textureIndex ) )

		def _writeNormalFaceVertex(outFile, faceIndex, faceVertexIndex):
			vertexIndex, textureIndex, normalIndex = product.getFaceVertex( faceIndex, faceVertexIndex )
			normalIndex += vertexNormalIndexOffsets[vertexIndex]
			outFile.write( '#   %d\n'  %  ( normalIndex ) )


		outFile.write( '  coordIndex [\n')
		for faceIndex in xrange( 0, product.getNumFaces() ):
			if bTriangulate:
				indexTris = product.getFaceIndexTriangles( faceIndex )
				for tri in indexTris:
					_writeFaceVertex( outFile, faceIndex, tri.a )
					_writeFaceVertex( outFile, faceIndex, tri.b )
					_writeFaceVertex( outFile, faceIndex, tri.c )
					outFile.write( '   -1 \n' )
			else:
				for faceVertexIndex in xrange( 0, product.getFaceSize( faceIndex ) ):
					_writeFaceVertex( outFile, faceIndex, faceVertexIndex )
				outFile.write( '   -1 \n' )
		outFile.write( '  ]\n')

		outFile.write( '  texCoordIndex [\n')
		for faceIndex in xrange( 0, product.getNumFaces() ):
			for faceVertexIndex in xrange( 0, product.getFaceSize( faceIndex ) ):
				_writeTexFaceVertex( outFile, faceIndex, faceVertexIndex )
		outFile.write( '  ]\n')

		outFile.write( '#  normalPerVertex TRUE\n')
		outFile.write( '#  normalIndex [\n')
		for faceIndex in xrange( 0, product.getNumFaces() ):
			for faceVertexIndex in xrange( 0, product.getFaceSize( faceIndex ) ):
				_writeNormalFaceVertex( outFile, faceIndex, faceVertexIndex )
		outFile.write( '#  ]\n')

		outFile.write( ' }\n')
		outFile.write( '}\n')

		return totalVertices + numVertices, totalVertexTextures + numVertexTextures, totalVertexNormals + numVertexNormals



def exportProduct(f, product, name, bTriangulate):
	"""Export a product in VRML97 format to a file
	@f - the file
	@product - the product to export
	@bTriangulate - triangulate all faces if True"""
	_writeProduct( f, product, name, bTriangulate, 0, 0, 0 )

def exportProducts(f, productsAndNames, bTriangulate):
	"""Export a list of products in VRML97 format to a file
	@f - the file
	@productsAndNames - a list of ( product, name ) pairs to export
	@bTriangulate - triangulate all faces if True"""
	totalVertices = 0
	totalVertexTextures = 0
	totalVertexNormals = 0

	for product, name in productsAndNames:
		totalVertices, totalVertexTextures, totalVertexNormals = _writeProduct( f, product, name, bTriangulate, totalVertices, totalVertexTextures, totalVertexNormals )
		f.write( '\n\n\n' )



def exportNode(f, node, bTriangulate):
	"""Export the result product of a node, in VRML97 format to a file
	@f - the file
	@node - the node whose product is to be exported
	@bTriangulate - triangulate all faces if True"""
	_writeProduct( f, node.getResultProduct(), node.name, bTriangulate, 0, 0, 0 )

def exportNodes(f, nodes, bTriangulate):
	"""Export the result products of a list of nodes, in VRML97 format to a file
	@f - the file
	@nodes - a list of nodes whose products are to be exported
	@bTriangulate - triangulate all faces if True"""
	totalVertices = 0
	totalVertexTextures = 0
	totalVertexNormals = 0

	for node in nodes:
		totalVertices, totalVertexTextures, totalVertexNormals = _writeProduct( f, node.getResultProduct(), node.name, bTriangulate, totalVertices, totalVertexTextures, totalVertexNormals )
		f.write( '\n\n\n' )

