##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Point3, Point4, Vector3, Point2, Point2f
from Britefury.Mesh.Mesh import GSProductMesh, MImportMesh
from Britefury.SceneNodes.SceneModelNode import SceneModelNode
from Britefury.ImportExportFilter.ToolImportedModel import ProcImportedModel
from Britefury.ImportExportFilter.Obj import ObjImport




class _ObjEntity (object):
	def __init__(self, name):
		self.name = name
		if self.name[0] == '"':
			self.name = self.name[1:]
		if self.name[-1] == '"':
			self.name = self.name[:-1]
		self.name = self.name.replace( '\\"', '"' )
		self.v = []
		self.vt = []
		self.vn = []
		self.f = []
		self.vToIndex = {}
		self.vtToIndex = {}
		self.vnToIndex = {}


	def addV(self, v):
		self.vToIndex[v] = len( self.v )
		self.v.append( v )

	def addVT(self, vt):
		self.vtToIndex[vt] = len( self.vt )
		self.vt.append( vt )

	def addVN(self, vn):
		self.vnToIndex[vn] = len( self.vn )
		self.vn.append( vn )



class _ObjImporter (object):
	def __init__(self, f, bMerge):
		self.default = _ObjEntity( 'default' )
		self.entities = [ self.default ]
		self.v = []
		self.vt = []
		self.vn = []
		self.current = self.default

		for line in f.readlines():
			line = line.strip()
			if line != '':
				data = line.split( None, 1 )
				if len( data ) >= 2:
					command, parameters = data
					parameters = parameters.strip()
					if command == 'g':
						if not bMerge:
							name = parameters
							entity = _ObjEntity( name )
							self.entities.append( entity )
							self.current = entity
					elif command == 'v':
						self.v.append( self.readV( parameters ) )
					elif command == 'vt':
						self.vt.append( self.readVT( parameters ) )
					elif command == 'vn':
						self.vn.append( self.readVN( parameters ) )
					elif command == 'f':
						self.current.f.append( self.readF( parameters ) )


	def readV(self, parameters):
		p = parameters.split()
		v = None
		try:
			if len( p ) == 3:
				v = Point4( float( p[0] ), float( p[1] ), float( p[2] ), 1.0 )
			elif len( p ) == 4:
				v = Point4( float( p[0] ), float( p[1] ), float( p[2] ), float( p[3] ) )
		except ValueError:
			v = Point4()
		return v

	def readVT(self, parameters):
		p = parameters.split()
		v = None
		try:
			if len( p ) == 2:
				v = Point3( float( p[0] ), float( p[1] ), 0.0 )
			elif len( p ) == 3:
				v = Point3( float( p[0] ), float( p[1] ), float( p[2] ) )
		except ValueError:
			v = Point3()
		return v

	def readVN(self, parameters):
		p = parameters.split()
		v = None
		try:
			if len( p ) == 3:
				v = Vector3( float( p[0] ), float( p[1] ), float( p[2] ) )
		except ValueError:
			v = Vector3()
		return v


	def strToObjIndex(self, s):
		if s == '':
			return None
		else:
			try:
				return int( s )
			except ValueError:
				return None

	def indexIntoObjValueList(self, l, index):
		if index is None  or  index == 0:
			return None
		elif index > 0:
			return l[index-1]
		else:
			return l[index]


	def readF(self, parameters):
		vertGroups = parameters.split()
		f = []
		for vertGroup in vertGroups:
			objIndices = [ self.strToObjIndex( s )   for s in vertGroup.split( '/' ) ]
			if len( objIndices ) == 1:
				vertexIndex = objIndices[0]
				texIndex = None
				normalIndex = None
			elif len( objIndices ) == 2:
				vertexIndex, texIndex = objIndices
				normalIndex = None
			elif len( objIndices ) == 3:
				vertexIndex, texIndex, normalIndex = objIndices
			else:
				continue

			vertex = self.indexIntoObjValueList( self.v, vertexIndex )
			tex = self.indexIntoObjValueList( self.vt, texIndex )
			normal = self.indexIntoObjValueList( self.vn, normalIndex )

			if vertex not in self.current.vToIndex  and  vertex is not None:
				self.current.addV( vertex )

			if tex not in self.current.vtToIndex  and  tex is not None:
				self.current.addVT( tex )

			if normal not in self.current.vnToIndex  and  normal is not None:
				self.current.addVN( normal )


			if vertex is None:
				vertex = -1
			else:
				vertex = self.current.vToIndex[vertex]

			if tex is None:
				tex = -1
			else:
				tex = self.current.vtToIndex[tex]

			if normal is None:
				normal = -1
			else:
				normal = self.current.vnToIndex[normal]

			f.append( ( vertex, tex, normal ) )
		return f



def importProducts(filename, bMerge):
	"""Import the products from an OBJ file
	@filename - the file name
	@bMerge - merge all models into one product if True"""
	f = open( filename, 'r' )
	importer = _ObjImporter( f, bMerge )

	namesAndProducts = []

	for entity in importer.entities:
		if len( entity.f ) > 0:
			product = GSProductMesh()
			importMesh = MImportMesh()

			for v in entity.v:
				importMesh.addVertex( v.toPoint3() )

			for t in entity.vt:
				importMesh.addTextureCoord( Point2f( t.toPoint2() ) )

			for f in entity.f:
				f = [ ( v, vt )  for v, vt, vn in f ]
				importMesh.addFace( f )

			importMesh.finalise()

			product.importMesh( importMesh )

			namesAndProducts.append( ( entity.name, product ) )
	
	f.close();

	return namesAndProducts




def importProducts(filename, bMerge):
	"""Import the products from an OBJ file
	@filename - the file name
	@bMerge - merge all models into one product if True"""
	if bMerge:
		product = GSProductMesh()
		importMesh = ObjImport.importObjFileAsSingleMesh( filename )
		product.importMesh( importMesh )
		
		return [ ( 'default', product ) ]
	else:
		namesAndMeshes = ObjImport.importObjFileAsMultipleMeshes( filename )
		namesAndProducts = []
		for name, mesh in namesAndMeshes:
			product = GSProductMesh()
			product.importMesh( mesh )
			namesAndProducts.append( ( name, product ) )
		return namesAndProducts




def importNodes(f, bMerge):
	"""Import the products from an OBJ file, and create scene nodes from them
	Each node will be a @SceneModelNode (from Britefury.SceneNodes.SceneModelNode), with a plan containing one @ProcImportedModel (from Britefury.ImportExportFilter.ToolImportedModel) procedure
	@f - the file
	@bMerge - merge all models into one node if True"""
	products = importProducts( f, bMerge )

	nodes = []

	for name, product in products:
		if bMerge  and  len( products ) == 1:
			name = 'obj_default'

		importedModelProc = ProcImportedModel()
		importedModelProc.model = product

		sceneNode = SceneModelNode()
		sceneNode.name = name
		sceneNode.plan.append( importedModelProc )

		nodes.append( sceneNode )

	return nodes
