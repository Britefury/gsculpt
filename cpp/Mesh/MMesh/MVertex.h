//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MVERTEX_H__
#define MVERTEX_H__

#include <stdio.h>

#include <algorithm>
#include <limits>


#include <Util/Array.h>
#include <Util/ArrayMap.h>
#include <Util/InPlaceAllocator.h>
#include <Util/Pool.h>
#include <Util/UniqueID.h>
#include <Util/SETree.h>

#include <Math/Point3.h>
#include <Math/Point2f.h>
#include <Math/Vector3f.h>
#include <Math/clamp.h>


#include <Graphics/Region.h>
#include <Graphics/BackfaceCull.h>

#include <Model/MarkPredicate.h>

#include <Mesh/MMesh/MeshTypes.h>
#include <Mesh/MMesh/MDijkstra.h>





/****************************************************************************
 ****************************************************************************
											MVertexAttrib
 ****************************************************************************
 ****************************************************************************/

class GS_DllExport MVertexAttrib
{
private:
	static Pool<MVertexAttrib> vertexAttribPool;


	Point2f point;								//8		(8)		(1 * 8)
	Point2f pointSave;							//8		(16)		(2 * 8)
	int materialID;								//4		(20)		(2.5 * 8)
	MVertex *vertex;							//4		(24)		(3 * 8)
	unsigned short refCount;					//2		(26)		(3.25 * 8)

public:
	short index;								//2		(28)		(3.5 * 8)


public:
	inline MVertexAttrib() : materialID( -1 ), vertex( NULL ), refCount( 0 ), index( -1 )
	{
	}

	inline MVertexAttrib(int ndx)
					 : materialID( -1 ), vertex( NULL ), refCount( 0 ), index( ndx )
	{
	}

	inline MVertexAttrib(const Point2f &p, int mat)
					: point( p ), pointSave( p ), materialID( mat ), vertex( NULL ), refCount( 0 ), index( -1 )
	{
	}



	void * operator new(size_t sz);
	void operator delete(void *data);
	static void poolAllocReserve(int numItmes);



	inline void init(const Point2f &p, int mat)
	{
		point = p;
		materialID = mat;
	}

	inline void init(const MVertexAttrib &attrib)
	{
		point = attrib.point;
		materialID = attrib.materialID;
	}


	inline void copyFrom(const MVertexAttrib *attrib, MVertex *owner)
	{
		point = attrib->point;
		pointSave = attrib->pointSave;
		materialID = attrib->materialID;
		vertex = owner;
		refCount = 0;
	}

	inline void read(FILE *f, MVertex *owner)
	{
		point.read( f );
		pointSave = point;
		fread( &materialID, sizeof(int), 1, f );
		vertex = owner;
		refCount = 0;
	}

	inline void write(FILE *f) const
	{
		point.write( f );
		fwrite( &materialID, sizeof(int), 1, f );
	}


	inline void read(SENode &node, MVertex *owner)
	{
		std::string header;
		node[0]  >>  header;
		if ( header == "vt" )
		{
			point.read( node[1] );
			pointSave = point;
			node[2]  >> materialID;
			vertex = owner;
			refCount = 0;
		}
	}

	inline void write(SENode &node) const
	{
		node.addItem()  <<  "vt";
		point.write( node.addList() );
		node.addItem()  <<  materialID;
	}



	inline void setVertex(MVertex *vertex)
	{
		this->vertex = vertex;
	}

	inline const MVertex * getVertex() const
	{
		return vertex;
	}


	inline int getRefCount() const
	{
		return refCount;
	}


	inline void setPoint(const Point2f &p)
	{
		point = p;
	}

	inline const Point2f & getPoint() const
	{
		return point;
	}

	inline void savePoint()
	{
		pointSave = point;
	}

	inline const Point2f &getSavedPoint() const
	{
		return pointSave;
	}

	inline void restorePoint()
	{
		point = pointSave;
	}


	inline int getMaterialID() const
	{
		return materialID;
	}

	inline void setMaterialID(int m)
	{
		materialID = m;
	}


	inline bool operator==(const MVertexAttrib &a) const
	{
		return point == a.point  &&  materialID == a.materialID;
	}



	void destroy();


	inline short getIndex() const
	{
		return index;
	}


	inline void ref()
	{
		refCount++;
	}

	inline void unref()
	{
		refCount--;
		if ( refCount == 0  &&  vertex != NULL )
		{
			destroy();
		}
	}


	inline void increaseRefCount(int amount)
	{
		refCount += amount;
	}



	friend class GS_DllExport MMesh;
	friend class GS_DllExport MVertex;
};







/****************************************************************************
 ****************************************************************************
											MVertexNormal
 ****************************************************************************
 ****************************************************************************/

class GS_DllExport MVertexNormal
{
private:
	static Pool<MVertexNormal> vertexNormalPool;


	Vector3f normal;						//12		(12)		(1.5 * 8)


public:
	int index;								//4		(16)		(2 * 8)


	inline MVertexNormal(int ndx = 0)
					: index( ndx )
	{
	}


	void * operator new(size_t sz);
	void operator delete(void *data);
	static void poolAllocReserve(int numItmes);



	inline void init(const Vector3f &n)
	{
		normal = n;
	}


	inline void copyFrom(const MVertexNormal *n)
	{
		normal = n->normal;
	}

	inline void read(FILE *f)
	{
		normal.read( f );
	}

	inline void write(FILE *f) const
	{
		normal.write( f );
	}


	inline void read(SENode &node)
	{
		std::string header;
		node[0]  >>  header;
		if ( header == "vn" )
		{
			normal.read( node[1] );
		}
	}

	inline void write(SENode &node) const
	{
		node.addItem()  <<  "vn";
		normal.write( node.addList() );
	}


	inline void setNormal(const Vector3f &n)
	{
		normal = n;
	}

	inline Vector3f & getNormal()
	{
		return normal;
	}

	inline const Vector3f & getNormal() const
	{
		return normal;
	}


	inline int getIndex() const
	{
		return index;
	}
};







/****************************************************************************
 ****************************************************************************
												MVertexList
 ****************************************************************************
 ****************************************************************************/

class GS_DllExport MVertexList : public Array<MVertex*>
{
public:
	bool closedFlag;


	MVertexList();

	void close();
	void open();
	void setClosed(bool c);
	bool isClosed() const;

	bool hasVertex(const MVertex *v) const;


	BBox3 computeBBox() const;
	Point3 computeAverage() const;

	bool hasEdge(const MEdge *e) const;

	bool isEdgeInOrder(MEdge *e, int index) const;
};





/****************************************************************************
 ****************************************************************************
												MVertex
 ****************************************************************************
 ****************************************************************************/

class GS_DllExport MVertex
{
public:
	static Pool<MVertex> vertexPool;


	class GS_DllExport VertexNeighbourhood
	{
	public:
		Array<MFace*> faces;
		Array<MEdge*> edges;


		VertexNeighbourhood();
		~VertexNeighbourhood();



		inline bool isClosed() const
		{
			return edges.size() == faces.size();
		}

		inline bool isOpen() const
		{
			return edges.size()  ==  ( faces.size() + 1 );
		}

		bool hasMarkedEdges() const;

		//edge 0 and edge 1 are indicent to face 0:
		//returns true if edge 0 comes before edge 1 in face 0; in other words,
		//if the order of the edges in the faces and the order of the edges
		//in the @edges Array agrees
		bool isOrderConsistent() const;

		//Determine if the edges and faces come in a counter-clockwise ordering
		//Edge 0 and edge 1 are incident to face 0:
		//returns true if edge 0 comes after edge 1 in face 0
		bool isCounterClockwise() const;

		void flip();
	};

	struct _BevelledEdgeQuadStrip
	{
		MVertexList vertices;
		Array<double> textureV;
		Array<int> edgeMaterials;
	};



	class GS_DllExport NeighbourhoodIterator
	{
	private:
		MVertex *v;
		MEdge *e;
		MFace *f;


	public:
		NeighbourhoodIterator(MVertex *v, MEdge *e, MFace *f);


		bool next();
		bool previous();


		inline MVertex * vertex()
		{
			return v;
		}

		inline MEdge * edge()
		{
			return e;
		}

		inline MFace * face()
		{
			return f;
		}
	};



private:
	class FaceExtrudedVertexTable
	{
	public:
		Array<MVertexList> vertices;

	private:
		typedef ArrayMap<MEdge*, MVertexList*> EdgeToVertexTable;
		typedef ArrayMap<MFace*, MVertexList*> FaceToVertexTable;

		EdgeToVertexTable edgeToVertex;
		FaceToVertexTable faceToVertex;


	public:
		inline FaceExtrudedVertexTable()
		{
		}


		inline void setVertexListForEdge(MEdge *e, MVertexList *v)
		{
			edgeToVertex.insert( e, v );
		}

		inline MVertexList * getVertexListForEdge(MEdge *e)
		{
			EdgeToVertexTable::iterator iter = edgeToVertex.find( e );

			if ( iter != edgeToVertex.end() )
			{
				return iter->second;
			}
			else
			{
				return NULL;
			}
		}


		inline void setVertexListForFace(MFace *f, MVertexList *v)
		{
			faceToVertex.insert( f, v );
		}

		inline MVertexList * getVertexListForFace(MFace *f)
		{
			FaceToVertexTable::iterator iter = faceToVertex.find( f );

			if ( iter != faceToVertex.end() )
			{
				return iter->second;
			}
			else
			{
				return NULL;
			}
		}
	};

	class BevelEdgeTable
	{
	public:
		struct CreatedBoundaryVertices
		{
			MVertexList vertices;
			int materialID;
			bool inverseOrderFlag;
		};

	private:
		typedef ArrayMap<MEdge*, MVertex*> EdgeToVertexTable;
		typedef ArrayMap<MFace*, MVertex*> FaceToVertexTable;

		EdgeToVertexTable edgeToVertex;
		FaceToVertexTable faceToVertex;


	public:
		Array<CreatedBoundaryVertices> createdBoundaryVerticesByNeighbourhood;
		Array<Point3> vertexFaceRelativePoints;

		inline BevelEdgeTable()
		{
		}


		inline void setVertexForEdge(MEdge *e, MVertex *v)
		{
			edgeToVertex.insert( e, v );
		}

		inline MVertex * getVertexForEdge(MEdge *e)
		{
			EdgeToVertexTable::iterator iter = edgeToVertex.find( e );

			if ( iter != edgeToVertex.end() )
			{
				return iter->second;
			}
			else
			{
				return NULL;
			}
		}


		inline void setVertexForFace(MFace *f, MVertex *v)
		{
			faceToVertex.insert( f, v );
		}

		inline MVertex * getVertexForFace(MFace *f)
		{
			FaceToVertexTable::iterator iter = faceToVertex.find( f );

			if ( iter != faceToVertex.end() )
			{
				return iter->second;
			}
			else
			{
				return NULL;
			}
		}
	};



	typedef InPlaceAllocator<MEdge*,4> EdgeListAlloc;
	typedef InPlaceAllocator<MFace*,4> FaceListAlloc;


	Array<MEdge*, EdgeListAlloc> edges;							//32  (32)	(4 * 8)
	Array<MFace*, FaceListAlloc> faces;							//32  (64)	(8 * 8)
	Array<MVertexAttrib*> vertexAttribs;							//12  (76)	(9.5 * 8)
	mutable Array<MVertexNormal*> vertexNormals;					//12	(88)	(11 * 8)

	Point3 position;											//24  (112)	(14 * 8)
	Point3 positionSave;										//24  (136)	(17 * 8)

	MVertexAttrib attrib0;										//32  (168)	(21 * 8)
	mutable MVertexNormal normal0;								//16	(184)	(23 * 8)
	float sharpness;											//4	(188)	(23.5 * 8)

	MMesh *mesh;											//4   (192)	(24 * 8)



	union													//8	(200) (25 * 8)
	{
		struct
		{
			union
			{
				MVertexList *single;
				FaceExtrudedVertexTable *multiple;
			} data;

			bool multipleSectionsFlag;
		} faceExtrude;

		struct
		{
			MVertex *duplicateVertex;
		} faceDuplicate;

		struct
		{
			float averageEdgeSharpness;
		} subdivision;

		struct
		{
			Vector3 *expansionVector;
		} expandEdge;

		struct
		{
			BevelEdgeTable *table;
		} bevelEdge;

		struct
		{
			Vector3f *averageNormal;
		} uvmap;

		struct
		{
			MDijkstraNode<MVertex, MEdge> *node;
		} dijkstra;
	} vertexOpData;



	struct Flags												//4	(204) (25.5 * 8)
	{
		unsigned vertexMark : 1;
		unsigned destroyed : 1;
		unsigned indestructible : 1;
		unsigned secondaryMark : 1;
		unsigned normalSharp : 1;
		unsigned positionChangedForMesh : 1;
		unsigned transformed : 1;
		mutable unsigned normalsRequireRefresh : 1;
		unsigned vertexMarkSave : 1;
		mutable unsigned topologyKnown : 1;
		mutable unsigned topologySimple : 1;
		unsigned liveSubdRequiresRefresh : 1;
		unsigned liveSubdVertexRequiresRefresh : 1;
		unsigned liveSubdEdgeSharpnessRequiresRefresh : 1;
		unsigned liveSubdNormalSharpnessModified : 1;
		unsigned debug : 1;
	} flags;

public:
	int index;													//4	(208) (26 * 8)



public:
	MVertex(MMesh *m, int ndx);
	~MVertex();


	void * operator new(size_t sz);
	void operator delete(void *data);
	static void poolAllocReserve(int numItmes);


	inline void init(const Point3 &pos)
	{
		position = pos;
		flags.positionChangedForMesh = true;
	}

	void copyFrom(const MVertex *vertex, MMesh *parentMesh, int edgesOffset, int facesOffset);


	inline void copyPropertiesFrom(const MVertex *v)
	{
		setVertexMark( v->isVertexMarked() );
		flags.secondaryMark = v->flags.secondaryMark;
		flags.normalSharp = v->flags.normalSharp;
		sharpness = v->sharpness;
	}

	inline void copyPropertiesFrom(const MVertex *v0, const MVertex *v1)
	{
		setVertexMark( v0->isVertexMarked()  ||  v1->isVertexMarked() );
		flags.secondaryMark = v0->flags.secondaryMark  &&  v1->flags.secondaryMark;
		flags.normalSharp = v0->flags.normalSharp  ||  v1->flags.normalSharp;
		sharpness = std::max( v0->sharpness, v1->sharpness );
	}

	void read(FILE *f, MMesh *parentMesh);
	void write(FILE *f);
	void read(SENode &node, MMesh *parentMesh);
	void write(SENode &node);


	void preComputeFull();




	void optimiseMemoryUsage();



	inline MMesh * getMesh() const
	{
		return mesh;
	}

	inline int getIndex() const
	{
		return index;
	}

	void meshShutdown();


	inline static int estimateCost()
	{
		return sizeof(MVertex) + sizeof(MVertexAttrib) + sizeof(MVertexNormal) + 40;
	}







	inline void clearLinks()
	{
		edges.clear();
		faces.clear();
		topologyModified();
	}


	//************************ POSITION OPERATIONS ************************
	//*********************************************************************
	inline const Point3 & getPosition() const
	{
		return position;
	}

	inline void clearPositionChangedForMeshFlag()
	{
		flags.positionChangedForMesh = false;
	}

	inline void clearTransformedFlag()
	{
		flags.transformed = false;
	}

	inline bool isTransformed() const
	{
		return flags.transformed;
	}

	inline void savePosition()
	{
		positionSave = position;

		for (int i = 0; i < vertexAttribs.size(); i++)
		{
			vertexAttribs[i]->savePoint();
		}
	}

	inline const Point3 & getSavedPosition() const
	{
		return positionSave;
	}

	void restorePosition();

	void setPosition(const Point3 &p);

	void translate(const Vector3 &translation);

	void transformPosition(const Matrix4 &x);

	void i_transformPosition(const Matrix4 &x, bool fromSaved);



	//************************** EDGE OPERATIONS **************************
	//*********************************************************************
	void addEdge(MEdge *e);
	void removeEdge(MEdge *e);

	inline bool _hasEdge(const MEdge *e) const
	{
		//return true if e can be found in edges
		for (int i = 0; i < edges.size(); i++)
		{
			if ( edges[i] == e )
			{
				return true;
			}
		}
		return false;
	}

	inline int getValence() const
	{
		return edges.size();
	}

	inline const MEdge * getEdge(int i) const
	{
		return edges[i];
	}

	inline MEdge * getEdge(int i)
	{
		return edges[i];
	}


	bool isIncidentTo(const MEdge *e) const;

	MEdge * findEdgeTo(MVertex *v, MFindPolicy findPolicy);
	MEdge * getBoundaryEdge() const;
	MEdge * getOtherBoundaryEdge(MEdge *e);
	MEdge * getNextBoundaryEdge(MEdge *e);



	//************************** FACE OPERATIONS **************************
	//*********************************************************************
	void addFace(MFace *f);
	void removeFace(MFace *f);

	inline int findFace(const MFace *f) const
	{
		//return the index if f can be found in faces, else return -1
		for (int i = 0; i < faces.size(); i++)
		{
			if ( faces[i] == f )
			{
				return i;
			}
		}
		return -1;
	}

	inline bool isIncidentTo(const MFace *f) const
	{
		return findFace( f )  !=  -1;
	}

	inline MFace* getFace(int i)
	{
		return faces[i];
	}

	inline const MFace* getFace(int i) const
	{
		return faces[i];
	}

	inline int getNumFaces() const
	{
		return faces.size();
	}


	inline MFace * findCommonFaceWith(MVertex *v)
	{
		for (int i = 0; i < faces.size(); i++)
		{
			if ( v->isIncidentTo( faces[i] ) )
			{
				return faces[i];
			}
		}

		return NULL;
	}

	inline void findCommonFacesWith(const MVertex *v, MFaceSet &commonFaces)
	{
		for (int i = 0; i < faces.size(); i++)
		{
			if ( v->isIncidentTo( faces[i] ) )
			{
				commonFaces.insert( faces[i] );
			}
		}
	}

	inline int getNumCommonFacesWith(const MVertex *v)
	{
		int numCommonFaces = 0;

		for (int i = 0; i < faces.size(); i++)
		{
			if ( v->isIncidentTo( faces[i] ) )
			{
				numCommonFaces++;
			}
		}

		return numCommonFaces;
	}

	inline static MFace* findCommonFaceAmong(Array<MVertex*> &verts)
	{
		//less than two vertices?
		if ( verts.size() < 2 )
		{
			//no common face
			return NULL;
		}

		for (int i = 0; i < verts[0]->faces.size(); i++)
		{
			MFace *f = verts[0]->faces[i];

			for (int j = 1; j < verts.size(); j++)
			{
				//check to see if @f is incident to the remaining vertice
				//in @verts
				if ( !verts[j]->isIncidentTo( f ) )
				{
					//@f is not incident to the vertex; set @f to NULL and break
					f = NULL;
					break;
				}
			}

			//if @f is non-NULL, we have a common face

			if ( f != NULL )
			{
				return f;
			}
		}

		return NULL;
	}


	Vector3 computeAverageNormal() const;

	MFace * findCommonFaceWithVertexNotIncidentToEdge(MVertex *v, MEdge *e);

	MFace * findBestSplitFace(MVertex *splitTo, bool allowDegenerateSubFaces = true);
	MFace * findBestSplitFace(const MVertexList &splitTargets);

	bool faceSplit(MVertex *vb, MFace *&faceA, MFace *&faceB, MEdge *&newEdge);

	int getFaceCapacityForEdgeTo(MVertex *vertex, MEdge *&edge);



	//******************** VERTEX ATTRIBUTE OPERATIONS ********************
	//*********************************************************************
	inline MVertexAttrib * getVertexAttrib0()
	{
		return &attrib0;
	}

	inline const MVertexAttrib * getVertexAttrib0() const
	{
		return &attrib0;
	}

	inline MVertexAttrib * getVertexAttrib(int i)
	{
		return vertexAttribs[i];
	}

	inline const MVertexAttrib * getVertexAttrib(int i) const
	{
		return vertexAttribs[i];
	}

	inline int getNumVertexAttribs() const
	{
		return vertexAttribs.size();
	}


	MVertexAttrib * createVertexAttrib(const MVertexAttrib &attrib, MFindPolicy findPolicy);
	void removeVertexAttrib(MVertexAttrib *attrib);
	MVertexAttrib * replaceVertexAttrib(MVertexAttrib *oldAttrib, MVertexAttrib *newAttrib);



	//********************** VERTEX NORMAL OPERATIONS *********************
	//*********************************************************************
	inline bool isNormalSharp() const
	{
		return flags.normalSharp;
	}

	inline void setNormalsRequireRefreshFlag()
	{
		flags.normalsRequireRefresh = true;
	}

	inline void clearNormalsRequireRefreshFlag()
	{
		flags.normalsRequireRefresh = false;
	}

	inline bool isNormalRefreshRequired() const
	{
		return  flags.normalsRequireRefresh;
	}


	inline MVertexNormal * getVertexNormal0()
	{
		if ( flags.normalsRequireRefresh )
		{
			refreshVertexNormals();
		}

		return &normal0;
	}

	inline MVertexNormal * getVertexNormal(int i)
	{
		if ( flags.normalsRequireRefresh )
		{
			refreshVertexNormals();
		}

		return vertexNormals[i];
	}

	inline int getNumVertexNormals()
	{
		if ( flags.normalsRequireRefresh )
		{
			refreshVertexNormals();
		}

		return vertexNormals.size();
	}

	inline MVertexNormal * _getVertexNormalNoRefresh(int i)
	{
		return vertexNormals[i];
	}

	inline int _getNumVertexNormalsNoRefresh()
	{
		return vertexNormals.size();
	}

	void setNormalSharp();
	void setNormalSmooth();
	void setNormalSharpness(bool s);
	void touchNormalSharpness();
	MVertexNormal * newVertexNormal(const Vector3f &n = Vector3f());
	void destroyVertexNormals();

private:
	void computeSectionNormals(MEdge *e);
public:
	void refreshVertexNormals();



	//************************ TOPOLOGY OPERATIONS ************************
	//*********************************************************************
	inline bool isExtraordinary() const
	{
		return edges.size() != 4;
	}

	inline bool isOrdinary() const
	{
		return edges.size() == 4  &&  faces.size() == 4;
	}


	inline bool isTopologySimple() const
	{
		refreshTopology();

		return flags.topologySimple;
	}


	inline NeighbourhoodIterator neighbourhoodIterator(MEdge *e, MFace *f = NULL)
	{
		return NeighbourhoodIterator( this, e, f );
	}


	int computeBoundaryCount() const;
	bool isOnBoundary() const;

	bool isOnOrdinaryBoundary() const;
	bool isOnOrdinaryCorner() const;

	void discoverNeighbourhoodFromEdge(MEdge *e, MFace *f, MEdgeList *edgeList, MFaceList *faceList);
	void discoverNeighbourhoodFromEdge(MEdge *e, MFace *f, VertexNeighbourhood &neighbourhood);
	void computeNeighbourhoodSize(MEdge *e, MFace *f, int &numEdges, int &numFaces) const;

	void discoverAllNeighbourhoods(Array<VertexNeighbourhood> &neighbourhoods);

	void topologyModified();

private:
	void refreshTopology() const;

public:



	//************************* DESTROY OPERATIONS ************************
	//*********************************************************************
	inline void markAsDestroyed()
	{
		flags.destroyed = true;
	}

	inline bool isDestroyed() const
	{
		return flags.destroyed;
	}


	inline void setIndestructible()
	{
		flags.indestructible = true;
	}

	inline void setDestructible()
	{
		flags.indestructible = false;
	}

	inline bool isIndestructible()
	{
		return flags.indestructible;
	}


	inline bool isIsolated() const
	{
		return edges.size() == 0;
	}


	//returns true if destroyed
	bool destroyIfIsolated();


	//*********************** SHARPNESS OPERATIONS ************************
	//*********************************************************************
	void setSharpness(double s);

	inline void setSharpness(double s, MSharpenFunction func)
	{
		switch ( func )
		{
		case MSHARPENFUNC_SET:
			setSharpness( s );
			break;
		case MSHARPENFUNC_ACCUMULATE:
			setSharpness( sharpness + s );
			break;
		default:
			break;
		}
	}

	inline float getSharpness() const
	{
		return sharpness;
	}

	inline bool isSharp() const
	{
		return sharpness > 0.0;
	}

	int computeCreasedEdgeCount() const;



	//********************** VERTEX MARK OPERATIONS ***********************
	//*********************************************************************
	void vertexMark();
	void vertexUnmark();
	void vertexMarkInvert();
	void setVertexMark(bool mark);

	inline void setVertexMark(MarkPredicate pred, bool inRegion)
	{
		setVertexMark( markPredicateIsMarked( pred, flags.vertexMark, inRegion ) );
	}

	inline bool isVertexMarked() const
	{
		return flags.vertexMark;
	}

	inline void vertexMarkSave()
	{
		flags.vertexMarkSave = flags.vertexMark;
	}

	inline void vertexMarkRestore()
	{
		setVertexMark( flags.vertexMarkSave );
	}

	bool hasNeighbouringMarkedVertices() const;
	bool hasNeighbouringUnmarkedVertices() const;

	void discoverVertexMarkedRegion(MVertexList &region);


	//********************* SECONDARY MARK OPERATIONS *********************
	//*********************************************************************
	inline void secondaryMark()
	{
		flags.secondaryMark = true;
	}

	inline void secondaryUnmark()
	{
		flags.secondaryMark = false;
	}

	inline bool isSecondaryMarked() const
	{
		return flags.secondaryMark;
	}



	//************************ EDGE MARK OPERATIONS ***********************
	//*********************************************************************
	bool isEdgeMarked() const;
	int getMarkedEdgeCount() const;
	MEdge * getMarkedEdge() const;
	MEdge * getMarkedEdgeSecondaryUnmarked() const;
	MEdge * getOtherMarkedEdge(const MEdge *e) const;
	MEdge * getOtherUnmarkedEdge(const MEdge *e) const;
	void getMarkedEdgeList(MEdgeList &edgeList) const;

	void discoverEdgeMarkedRegionVertices(MVertexList &region);
	void discoverEdgeMarkedRegionVertices(MVertexList &regionVertices, MEdgeList &regionEdges);

	//returns:
	//		true if last edge encountered is edge marked, and is not the same as @e,
	//		false if last edge encountered is a boundary edge, or is the same as @e
	bool discoverNeighbourhoodBetweenMarkedEdges(MEdge *e, MFace *f, MEdgeList *edgeList, MFaceList *faceList, MEdge *boundaryEdges[2]);

	//returns:
	//		true if last edge encountered is edge marked, and is not the same as @e,
	//		false if last edge encountered is a boundary edge, or is the same as @e
	bool discoverNeighbourhoodBetweenMarkedEdges(MEdge *e, MFace *f, VertexNeighbourhood &neighbourhood, MEdge *boundaryEdges[2]);

	MEdge * traverseToOtherMarkedEdge(MEdge *e);



	//************************ FACE MARK OPERATIONS ***********************
	//*********************************************************************
	bool isFaceMarked() const;
	int getFaceMarkBoundaryCount() const;
	bool isOnFaceMarkBoundary() const;
	bool isFaceMarkInternal() const;

private:
	// If @markedFlag is false, then UNMARKED neighbourhoods will be discovered
	void discoverFaceMarkNeighbourhood(MEdge *e, MEdgeList *edgeList, MFaceList *faceList, MEdge *boundaryEdges[2], bool markedFlag);
	void discoverFaceMarkNeighbourhood(MEdge *e, VertexNeighbourhood &neighbourhood, bool markedFlag);
	void discoverAllFaceMarkNeighbourhoods(Array<VertexNeighbourhood> &neighbourhoods, bool markedFlag);

public:
	inline void discoverAllFaceMarkedNeighbourhoods(Array<VertexNeighbourhood> &neighbourhoods)
	{
		discoverAllFaceMarkNeighbourhoods( neighbourhoods, true );
	}

	inline void discoverAllFaceUnmarkedNeighbourhoods(Array<VertexNeighbourhood> &neighbourhoods)
	{
		discoverAllFaceMarkNeighbourhoods( neighbourhoods, false );
	}



	MEdge * getOtherFaceMarkBoundaryEdge(MEdge *e);
	MEdge * getNextFaceMarkBoundaryEdge(MEdge *e);

	MFace * getFaceMarkedFace();

	void discoverFaceMarkedRegionVertices(MVertexList &region);
	void discoverFaceMarkedRegionVertices(MVertexList &regionVertices, MFaceList &regionFaces);




	//********************** PATH DISCOVERY OPERATIONS ***********************
	//*********************************************************************
	bool discoverEdgeLoopPathTo(MVertex *target, MEdgeRun &path);

	void discoverPathInit();
	void discoverPathFinish();


	inline MDijkstraNode<MVertex, MEdge> * getDijkstraNode()
	{
		return vertexOpData.dijkstra.node;
	}


private:
	void discoverPathSetCost(MDijkstraHeap<MVertex> &heap, double cumulativeCost, MVertex *previous, MEdge *edge);


public:
	//********************** SUBDIVISION OPERATIONS ***********************
	//*********************************************************************
	inline float getSubdivideAverageEdgeSharpness() const
	{
		return vertexOpData.subdivision.averageEdgeSharpness;
	}

private:
	float computeAverageEdgeSharpnessForSubdivision() const;
	float catmullClarkComputeEdgeSharpnessForVertexPos(MEdge *sharpEdges[2]);

	Point2f catmullClarkSmoothAttrib(MVertexAttrib *attrib);
	Point2f catmullClarkCreaseAttrib(MVertexAttrib *attrib);
	Point2f catmullClarkBoundaryAttrib(MVertexAttrib *attrib);
	void catmullClarkAttrib(MVertexAttrib *attrib);


public:
	void computeNewPositionCatmullClark(MPreserveNormalSharpness preserveNormalSharpness);
	void liveSubdivideReposition(MMesh *liveMesh, int edgeSubdVerticesOffset, int faceSubdVerticesOffset, MPreserveNormalSharpness preserveNormalSharpness);
	void refreshLiveSubdivision(MMesh *liveMesh, int edgeSubdVerticesOffset, int faceSubdVerticesOffset, MPreserveNormalSharpness preserveNormalSharpness);

	inline void liveSubdivisionIncidentFaceVertexModified()
	{
		flags.liveSubdVertexRequiresRefresh = true;
	}

	inline void liveSubdivisionIncidentEdgeVertexModified()
	{
		flags.liveSubdVertexRequiresRefresh = true;
	}

	inline void liveSubdivisionIncidentEdgeSharpnessModified()
	{
		flags.liveSubdEdgeSharpnessRequiresRefresh = true;
		flags.liveSubdVertexRequiresRefresh = true;
	}

	inline void liveSubdivisionIncidentEdgeNormalSharpnessModified()
	{
		flags.liveSubdNormalSharpnessModified = true;
	}

	void liveSubdivisionRefreshInit();

	inline void liveSubdivisionRefreshed()
	{
		flags.liveSubdRequiresRefresh = false;
		flags.liveSubdVertexRequiresRefresh = false;
		flags.liveSubdEdgeSharpnessRequiresRefresh = false;
		flags.liveSubdNormalSharpnessModified = false;
	}



	//********************** EXTRUDE FACE OPERATIONS **********************
	//*********************************************************************
private:
	void extrudeFaceCreateExtrudedVertices(MVertexList &verts, int numSegments);
	void pathExtrudeFaceCreateExtrudedVertices(MVertexList &verts, MPathExtrudeFaceMode mode, const Array<Matrix4> &matrices);
	void extrudeFaceExtrudeVertexAdjust(MVertexList &verts, MVertexAdjustList &vertexAdjusts, const Vector3 extrusionVector, int numSegments);
	void extrudeFaceExtrudeVertexAdjust(MVertexList &verts, MVertexVectorAdjustList &vertexAdjusts, int numSegments);
	void fillInExtrudeFaceMultipleSectionsTable(VertexNeighbourhood &neighbourhood, MVertexList *extruded);

public:
	void extrudeFaceExtrudeVertex(MVertexAdjustList &vertexAdjusts, MExtrudeFaceDirection direction, int numSegments);
	void extrudeFaceVectorExtrudeVertex(MVertexVectorAdjustList &vertexAdjusts, int numSegments);
	void pathExtrudeFaceExtrudeVertices(MPathExtrudeFaceMode mode);



	inline MVertexList * getExtrudeFaceExtrudedVertices(MEdge *e)
	{
		if ( vertexOpData.faceExtrude.multipleSectionsFlag )
		{
			return vertexOpData.faceExtrude.data.multiple->getVertexListForEdge( e );
		}
		else
		{
			return vertexOpData.faceExtrude.data.single;
		}
	}

	inline MVertexList * getExtrudeFaceExtrudedVertices(MFace *f)
	{
		if ( vertexOpData.faceExtrude.multipleSectionsFlag )
		{
			return vertexOpData.faceExtrude.data.multiple->getVertexListForFace( f );
		}
		else
		{
			return vertexOpData.faceExtrude.data.single;
		}
	}

	inline void extrudeFaceDeleteExtrudeData()
	{
		if ( vertexOpData.faceExtrude.multipleSectionsFlag )
		{
			delete vertexOpData.faceExtrude.data.multiple;
		}
		else
		{
			if ( vertexOpData.faceExtrude.data.single != NULL )
			{
				delete vertexOpData.faceExtrude.data.single;
			}
		}
	}



	//********************** DUPLICATE FACES OPERATIONS **********************
	//*********************************************************************
	void duplicateFacesCreateDuplicateVertex(MVertexVectorAdjustList &vertexAdjusts);

	inline MVertex * getDuplicateFacesDuplicateVertex()
	{
		return vertexOpData.faceDuplicate.duplicateVertex;
	}



	//*********************** INSET/EXPAND FACE OPERATIONS ***********************
	//*********************************************************************
public:
	void insetExpandFacesInsertCornerVertices(MEdgeList &edgesToCheck, MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts,
										MInsetFaceStyle insetStyle, bool flowThruMarkedEdges, MInsetExpandOperation insetExpandOp);
	void insetExpandFacesSplitIncidentInternalFaces(MInsetExpandOperation insetExpandOp, bool flowThruMarkedEdges, bool markBoundaryEdges, bool nsharpBoundaryEdges);
	void insetExpandFacesMarkedInternalVertexExtrude(MVertexAdjustList &vertexAdjust);


	//*********************** INSET FACE OPERATIONS ***********************
	//*********************************************************************
	void insetFacesSplitInternalEdges(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MInsetFaceStyle insetStyle, bool flowThruMarkedEdges,
								bool markInsetEdges, bool nsharpInsetEdges);


	//*********************** EXPAND FACE OPERATIONS ***********************
	//*********************************************************************
	void expandFacesSplitInternalEdges(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, bool markExpandEdges, bool nsharpExpandEdges);


	//EXPAND VERTEX OPERATIONS
	void expand(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, bool markBoundaryEdges, bool normalSharpenBoundaryEdges);


	//EXPAND VERTEX IN MARKED FACES OPERATIONS
	void expandInMarkedFaces(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, bool markBoundaryEdges);


	//BEVEL VERTEX OPERATIONS
private:
	Vector3 computeBevelVector(const MEdgeList &edgeList) const;

public:
	void bevel(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts,
			bool useExistingTexturing, bool markBoundaryEdges, bool normalSharpenBoundaryEdges);


	//EXPAND EDGE OPERATIONS
private:
	double computeExpandEdgeScaleFactor(MEdge *expandedEdge, MEdge *edgeToSplit, double epsilonSquared);
	static double computeExpandEdgeScaleFactorEpsilonSquared(const MEdgeList &edgeList);
public:
	void expandEdgeSplitUnmarkedEdges(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MEdgeList &boundaryEdges);
	void expandEdgeMoveVertex(MVertexAdjustList &vertexAdjusts);


	//BEVEL EDGE OPERATIONS
	void bevelEdgeInit(bool initialiseForBevel);
	void bevelEdgeFinish();
	bool isInitialisedForBevelEdge() const;
	MVertex * getBevelEdgeBoundaryVertexForEdge(MEdge *e);
	MVertex * getBevelEdgeBoundaryVertexForFace(MFace *f);
	void bevelEdgeCreateBoundaryVertices(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &attribAdjusts);
	void bevelEdgeCreateVertexFace(bool useExistingTexturing, MFaceList &createdFaces);
private:
	void bevelEdgeExtractQuadstrip(_BevelledEdgeQuadStrip &strip, MEdge *startEdge, bool storeEdgeMaterials);
public:
	void bevelEdgeExtractOpenQuadstrips(Array<_BevelledEdgeQuadStrip> &strips, bool storeEdgeMaterials);
	void bevelEdgeExtractClosedQuadstrips(Array<_BevelledEdgeQuadStrip> &strips, bool storeEdgeMaterials);


	//EXTRUDE EDGE OPERATIONS
	int getExtrudeableEdgeCount() const;
	MEdge * getExtrudeableEdge();
	MEdge * getOtherExtrudeableEdge(MEdge *e);
	MEdge * getNextExtrudeableEdge(MEdge *e);


	//SLIDE EDGE OPERATIONS
private:
	void slideEdgeCreateAdjusts(MEdge *eNegative, MEdge *ePositive, MFace *fNegative, MFace *fPositive,
							MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts);
	void slideEdgeCreateAdjusts(MEdge *eNegative, MEdge *ePositive, MFace *fNegativeA, MFace *fNegativeB, MFace *fPositiveA, MFace *fPositiveB,
							MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts);
	void slideEdgeCreateAdjusts(const MEdgeList &eNegative, const MEdgeList &ePositive, const MFaceList &fNegative, const MFaceList &fPositive,
							MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts);
public:
	void slideEdgeRunStartVertex(MEdge *e, MEdge *&eNegative, MEdge *&ePositive, MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts,
							int slideVersion);
	void slideEdgeRunEndVertex(MEdge *e, MEdge *eNegative, MEdge *ePositive, MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts,
							int slideVersion);
	void slideEdgeRunInternalVertex(MEdge *e, MEdge *&eNegative, MEdge *&ePositive, MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts,
							int slideVersion);



	//MARK EDGE LOOP OPERATIONS
	MEdge * markEdgeLoopGetOppositeEdge(MEdge *e);


	//KNIFE OPERATIONS
	void knife();



	//FACE SPLIT OPERATIONS
	void replaceFace(MFace *face, MFace *newFace);


	//REPLACE OPERATIONS
	void replaceWith(MVertex *vNew);


	//COLLAPSE / WELD OPERATIONS
public:
	static bool checkEdgeCollapseValidity(MEdge *e, bool markMergedEdges);
	static bool checkEdgeCollapseValidity(MVertex *va, MVertex *vb, MFace *f);

private:
	MVertex * weldTo(MVertex *v, const Point3 &mergedPosition, const MVertex *positionTarget, bool markMergedEdges);

public:
	MVertex * weldTo(MVertex *v, MWeldStyle style, bool markMergedEdges);
	MVertex * weldTo(MVertex *v, bool markMergedEdges);
	MVertex * weldToMidPointWith(MVertex *v, bool markMergedEdges);
	bool canWeldTo(MVertex *v);
	bool canWeldTo(MEdge *e);
	bool isLinkedTo(MVertex *v);


	//DISSOLVE OPERATIONS
	bool dissolve(bool twoEdgesOnly, bool twoFacesOnly, bool markCreatedFaces);
	bool dissolveSimpleValenceTwoVertex();			// If the vertex has only two edges and two faces, then dissolve it, combining the incident edges


	//TWEAK OPERATIONS
	void vertexTweakAlongSurfaceAdjustable(MVertexSurfaceTweakAdjust &adjust, const Vector3 &viewVector);
	void edgeTweakAlongSurfaceAdjustable(MVertexSurfaceTweakAdjust &adjust, const Vector3 &viewVector);
	void faceTweakAlongSurfaceAdjustable(MVertexSurfaceTweakAdjust &adjust, const Vector3 &viewVector);
	void vertexTweakAlongNormalAdjustable(MVertexAdjust &adjust);
	void edgeTweakAlongNormalAdjustable(MVertexAdjust &adjust);
	void faceTweakAlongNormalAdjustable(MVertexAdjust &adjust);



	//BOUNDARY / MARK OPERATIONS
	bool isFrontFacing(const BackfaceCull *faceCull) const;
	bool isInside(const Region3d *region) const;
	bool isInside(const Region3d *region, const BackfaceCull *faceCull) const;




	//RELAX OPERATIONS
	Point3 computeRelaxedPosition() const;




	//************************ GEOMETRY OPERATIONS ************************
	//*********************************************************************
	double distanceTo(const MVertex *v) const
	{
		return position.distanceTo( v->position );
	}

	double sqrDistanceTo(const MVertex *v) const
	{
		return position.sqrDistanceTo( v->position );
	}



	//************************ MATERIAL OPERATIONS ************************
	//*********************************************************************
	inline void transformMaterial(const Matrix4 &xform)
	{
		for (int i = 0; i < vertexAttribs.size(); i++)
		{
			MVertexAttrib *a = vertexAttribs[i];
			a->setPoint( a->getPoint() * xform );
		}
	}

	inline void transformMaterialSave()
	{
		for (int i = 0; i < vertexAttribs.size(); i++)
		{
			vertexAttribs[i]->savePoint();
		}
	}

	inline void transformMaterialRestore()
	{
		for (int i = 0; i < vertexAttribs.size(); i++)
		{
			vertexAttribs[i]->restorePoint();
		}
	}


	void uvmapInitialise();
	void uvmapShutdown();
	const Vector3f& getUVMapNormal() const;



	//************************ EVENTS / NOTIFICATIONS ************************
	//*********************************************************************
	inline void onFacePlaneModified()
	{
		setNormalsRequireRefreshFlag();
	}



	//DEBUG METHODS
	void display();
	bool checkIntegrity();


	friend class MFace;
};





/****************************************************************************
 ****************************************************************************
											MAdjustList
 ****************************************************************************
 ****************************************************************************/

template <class Adjust> class MAdjustList
{
protected:
	Array<Adjust> adjustments;

public:
	inline MAdjustList()
	{
	}


	inline int push_back(const Adjust &a)
	{
		return adjustments.push_back( a );
	}

	inline Adjust & push_back()
	{
		return adjustments.push_back();
	}

	inline void reserve(int r)
	{
		adjustments.reserve( r );
	}

	inline int size() const
	{
		return adjustments.size();
	}

	inline void clear()
	{
		adjustments.clear();
	}


	friend class GS_DllExport MMesh;
};



/****************************************************************************
 ****************************************************************************
											MVertexAdjust
 ****************************************************************************
 ****************************************************************************/

class GS_DllExport MVertexAdjust
{
private:
	MVertex *vtx;
	Point3 at0;
	Vector3 uMotion;
	double uLower, uUpper;
	Vector3 vMotion;
	double vLower, vUpper;

public:
	inline MVertexAdjust() : vtx( NULL )
	{
	}

	inline MVertexAdjust(MVertex *vertex, const Point3 &atZero, const Vector3 &u)
					: vtx( vertex ), at0( atZero ), uMotion( u ), uLower( -std::numeric_limits<double>::max() ), uUpper( std::numeric_limits<double>::max() ), vLower( 0.0 ), vUpper( 0.0 )
	{
	}

	inline MVertexAdjust(MVertex *vertex, const Point3 &atZero, const Vector3 &u, double uLower, double uUpper)
					: vtx( vertex ), at0( atZero ), uMotion( u ), uLower( uLower ), uUpper( uUpper ), vLower( 0.0 ), vUpper( 0.0 )
	{
	}

	inline MVertexAdjust(MVertex *vertex, const Point3 &atZero, const Vector3 &u, const Vector3 &v)
					: vtx( vertex ), at0( atZero ),
					uMotion( u ), uLower( -std::numeric_limits<double>::max() ), uUpper( std::numeric_limits<double>::max() ),
					vMotion( v ), vLower( -std::numeric_limits<double>::max() ), vUpper( std::numeric_limits<double>::max() )
	{
	}

	inline MVertexAdjust(MVertex *vertex, const Point3 &atZero, const Vector3 &u, double uLower, double uUpper, const Vector3 &v, double vLower, double vUpper)
					: vtx( vertex ), at0( atZero ), uMotion( u ), uLower( uLower ), uUpper( uUpper ), vMotion( v ), vLower( vLower ), vUpper( vUpper )
	{
	}


	inline MVertex* getVertex()
	{
		return vtx;
	}

	inline void setUMovementVector(const Vector3 &u)
	{
		uMotion = u;
	}

	inline void setVMovementVector(const Vector3 &v)
	{
		vMotion = v;
	}


	inline void setParameters(double alpha, double beta)
	{
		vtx->setPosition( at0  +  uMotion * clamp( alpha, uLower, uUpper )  +  vMotion * clamp( beta, vLower, vUpper ) );
	}
};


class GS_DllExport MVertexAdjustList : public MAdjustList<MVertexAdjust>
{
public:
	MVertexAdjustList();
	~MVertexAdjustList();

	void setParameters(double alpha, double beta);
};





/****************************************************************************
 ****************************************************************************
										MVertexVectorAdjust
 ****************************************************************************
 ****************************************************************************/


class GS_DllExport MVertexVectorAdjust
{
private:
	MVertex *vtx;
	Point3 initial;
	double scale;

public:
	inline MVertexVectorAdjust() : vtx( NULL )
	{
	}

	inline MVertexVectorAdjust(MVertex *vertex, const Point3 &initialPosition, double scaleFactor)
										: vtx( vertex ), initial( initialPosition ), scale( scaleFactor )
	{
	}


	inline MVertex* getVertex()
	{
		return vtx;
	}

	inline void setParameters(const Vector3 &v)
	{
		vtx->setPosition( initial  +  v * scale );
	}
};


class GS_DllExport MVertexVectorAdjustList : public MAdjustList<MVertexVectorAdjust>
{
public:
	MVertexVectorAdjustList();
	~MVertexVectorAdjustList();

	void setParameters(const Vector3 &v);
};





/****************************************************************************
 ****************************************************************************
							MVertexSlideAdjust
 ****************************************************************************
 ****************************************************************************/

class GS_DllExport MVertexSlideAdjust
{
private:
	MVertex *vtx;
	Point3 at0;
	Vector3 negative, positive;

public:
	inline MVertexSlideAdjust() : vtx( NULL )
	{
	}

	inline MVertexSlideAdjust(MVertex *vertex, const Point3 &atZero, const Vector3 &negative, const Vector3 &positive)
						: vtx( vertex ), at0( atZero ), negative( negative ), positive( positive )
	{
	}


	inline MVertex* getVertex()
	{
		return vtx;
	}

	inline void setNegativeVector(const Vector3 &v)
	{
		negative = v;
	}

	inline void setPositiveVector(const Vector3 &v)
	{
		positive = v;
	}


	inline void setSlideFactor(double slide)
	{
		if ( slide < 0.0 )
		{
			vtx->setPosition( at0  +  negative * -slide );
		}
		else if ( slide > 0.0 )
		{
			vtx->setPosition( at0  +  positive * slide );
		}
		else
		{
			vtx->setPosition( at0 );
		}
	}
};


class GS_DllExport MVertexSlideAdjustList : public MAdjustList<MVertexSlideAdjust>
{
public:
	MVertexSlideAdjustList();
	~MVertexSlideAdjustList();

	void setSlideFactor(double slide);
};





/****************************************************************************
 ****************************************************************************
					MVertexSurfaceTweakAdjust
 ****************************************************************************
 ****************************************************************************/


class GS_DllExport MVertexSurfaceTweakAdjust
{
private:
	struct Neighbourhood;

	class InputFace
	{
	public:
		Vector3 u, v;
		Vector3 planeNormal;
		int faceIndex;


		InputFace();
		InputFace(int faceIndex, const Vector3 &u, const Vector3 &v);
		InputFace(int faceIndex, const Vector3 &u, const Vector3 &v, const Vector3 &planeNormal);


		bool isPreviousTo(const InputFace &face, int neighbourhoodSize) const;
		bool isFrontFacing(const Vector3 &viewVector) const;
	};

	class FaceEntry
	{
	public:
		Vector3 planeNormal;
		Vector3 inBoundaryNormal, outBoundaryNormal;
		Vector3 u, v;


		FaceEntry();
		FaceEntry(const Vector3 &planeNormal, const Vector3 &u, const Vector3 &v, const Vector3 &viewVector);
	};

	struct Neighbourhood
	{
		Array<InputFace> inputFaces;
		Array<FaceEntry> faces;
		int vertexNeighbourhoodSize;
	};

	Point3 initialPosition;
	Array<Neighbourhood> neighbourhoods;
	MVertex *vertex;


public:
	MVertexSurfaceTweakAdjust();
	MVertexSurfaceTweakAdjust(MVertex *vertex, int numNeighbourhoods);

	void initNeighbourhood(int neighbourhoodIndex, int vertexNeighbourhoodSize);

private:
	void addInputFace(int neighbourhoodIndex, const InputFace &face, const Vector3 &viewVector);

public:
	void addFace(int neighbourhoodIndex, int faceIndex, const MFace *face, Vector3 u, Vector3 v, bool bUse, const Vector3 &viewVector);

	void build(const Vector3 &viewVector);


	Vector3 getConstrainedTweak(const Vector3 &v, const Vector3 &viewVector) const;
	void tweak(const Vector3 &v, const Vector3 &viewVector);

	void debugDraw();
};



class GS_DllExport MVertexSurfaceTweakAdjustList : public MAdjustList<MVertexSurfaceTweakAdjust>
{
private:
	Vector3 viewVector;

public:
	MVertexSurfaceTweakAdjustList();
	~MVertexSurfaceTweakAdjustList();

	void setViewVector(const Vector3 &viewVector);

	void tweak(const Vector3 &v);

	void debugDraw();
};





/****************************************************************************
 ****************************************************************************
											MVertexAttribAdjust
 ****************************************************************************
 ****************************************************************************/


class GS_DllExport MVertexAttribAdjust
{
private:
	MVertexAttrib *attr;
	Point2f at0;
	Vector2f motion;
	float uLower, uUpper;

public:
	inline MVertexAttribAdjust() : attr( NULL )
	{
	}

	inline MVertexAttribAdjust(MVertexAttrib *attrib, const Point2f &atZero, const Vector2f &u)
						: attr( attrib ), at0( atZero ), motion( u ), uLower( -std::numeric_limits<float>::max() ), uUpper( std::numeric_limits<float>::max() )
	{
	}

	inline MVertexAttribAdjust(MVertexAttrib *attrib, const Point2f &atZero, const Vector2f &u, float uLower, float uUpper)
						: attr( attrib ), at0( atZero ), motion( u ), uLower( uLower ), uUpper( uUpper )
	{
	}



	inline MVertexAttrib* getAttrib()
	{
		return attr;
	}


	inline void setParameters(float alpha)
	{
		attr->setPoint( at0  +  motion * clamp( alpha, uLower, uUpper ) );
	}
};



class GS_DllExport MVertexAttribAdjustList : public MAdjustList<MVertexAttribAdjust>
{
public:
	MVertexAttribAdjustList();
	~MVertexAttribAdjustList();

	void setParameters(float alpha);
};




/****************************************************************************
 ****************************************************************************
									MVertexAttribVectorAdjust
 ****************************************************************************
 ****************************************************************************/


class GS_DllExport MVertexAttribVectorAdjust
{
private:
	MVertexAttrib *attr;
	Point2f initial;
	float scale;

public:
	inline MVertexAttribVectorAdjust() : attr( NULL )
	{
	}

	inline MVertexAttribVectorAdjust(MVertexAttrib *attrib,
												const Point2f &initialPosition,
												float scaleFactor)
												: attr( attrib ), initial( initialPosition ),
												  scale( scaleFactor )
	{
	}



	inline MVertexAttrib* getAttrib()
	{
		return attr;
	}


	inline void setParameters(const Vector2f &v)
	{
		attr->setPoint( initial  +  v * scale );
	}
};




class GS_DllExport MVertexAttribVectorAdjustList : public MAdjustList<MVertexAttribVectorAdjust>
{
public:
	MVertexAttribVectorAdjustList();
	~MVertexAttribVectorAdjustList();

	void setParameters(const Vector2f &v);
};



/****************************************************************************
 ****************************************************************************
					MVertexAttribSlideAdjust
 ****************************************************************************
 ****************************************************************************/


class GS_DllExport MVertexAttribSlideAdjust
{
private:
	MVertexAttrib *attr;
	Point2f at0;
	Vector2f negative, positive;

public:
	inline MVertexAttribSlideAdjust() : attr( NULL )
	{
	}

	inline MVertexAttribSlideAdjust(MVertexAttrib *attrib, const Point2f &atZero, const Vector2f &negative, const Vector2f &positive)
								: attr( attrib ), at0( atZero ), negative( negative ), positive( positive )
	{
	}



	inline MVertexAttrib * getAttrib()
	{
		return attr;
	}


	inline void setSlideFactor(double slide)
	{
		if ( slide < 0.0 )
		{
			attr->setPoint( at0  +  negative * -slide );
		}
		else if ( slide > 0.0 )
		{
			attr->setPoint( at0  +  positive * slide );
		}
		else
		{
			attr->setPoint( at0 );
		}
	}
};



class GS_DllExport MVertexAttribSlideAdjustList : public MAdjustList<MVertexAttribSlideAdjust>
{
public:
	MVertexAttribSlideAdjustList();
	~MVertexAttribSlideAdjustList();

	void setSlideFactor(double slide);
};




/****************************************************************************
 ****************************************************************************
										MTransformationTarget
 ****************************************************************************
 ****************************************************************************/

class GS_DllExport MTransformationTarget
{
private:
	class XformGroup
	{
	public:
		MVertexList vertices;
		Point3 centre;

		inline void finalise()
		{
			centre = vertices.computeAverage();
		}
	};



	Array<XformGroup> groups;
	Point3 centre;
	MMesh *mesh;


public:
	MTransformationTarget();
	~MTransformationTarget();


	const Point3 & getCentre() const;

	int size() const;

	void translate(const Vector3 &translation);
	void centredTransform(const Matrix4 &xform);


private:
	void finalise();
	void clear();
	XformGroup & newGroup();
	void setMesh(MMesh *m);

	friend class GS_DllExport MMesh;
};


#endif

