//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BACKGROUNDMESH_H__
#define BACKGROUNDMESH_H__

#include <boost/python.hpp>

#include <PlatformSpecific/IncludeGL.h>

#include <PlatformSpecific/GLExtensions.h>

#include <Util/Array.h>
#include <Util/ProgressMonitor.h>

#include <Math/Point3f.h>
#include <Math/Vector3f.h>
#include <Math/BBox3f.h>
#include <Math/Segment3f.h>


#define _KDT_SPLIT_AXIS_SHIFT 28
#define _KDT_SPLIT_AXIS_MASK ( 3 << _KDT_SPLIT_AXIS_SHIFT )
#define _KDT_SPLIT_AXIS_INVMASK (~_KDT_SPLIT_AXIS_MASK)

#define _KDT_NODE_INDEX_MASK ( ( 1 << 28 ) - 1 )
#define _KDT_NODE_INDEX_INVMASK (~_KDT_NODE_INDEX_MASK)

#define _KDT_DIRFLAG_VALID_X 1
#define _KDT_DIRFLAG_VALID_Y 2
#define _KDT_DIRFLAG_VALID_Z 4

#define _KDT_DIRFLAG_POS_X 8
#define _KDT_DIRFLAG_POS_Y 16
#define _KDT_DIRFLAG_POS_Z 32


//#define _BACKGROUNDMESH_USE_VERTEX_BUFFERS
#define _BACKGROUNDMESH_USE_VERTEX_ARRAYS_AND_DISPLAY_LIST



class GS_DllExport BackgroundMesh
{
public:
	struct IndexTri
	{
		int i[3];
	};



private:
	class GS_DllExport KDTree
	{
	public:
		class KDSegment
		{
		public:
			Point3f a, b;
			Vector3f direction;

			inline KDSegment()
			{
			}

			inline KDSegment(const Point3f &a, const Point3f &b)
					: a( a ), b( b ), direction( b - a )
			{
			}


			inline Point3f getMidPoint() const
			{
				return Point3f::average( a, b );
			}

			inline Vector3f getDirection() const
			{
				return direction;
			}

			inline Point3f getPoint(float t) const
			{
				return a + direction * t;
			}


			inline void setEnd(const Point3f &end)
			{
				b = end;
				direction = b - a;
			}
		};


		class KDTTri
		{
		public:
			Point3f a;
			Vector3f e1, e2;
		
		
			inline KDTTri()
			{
			}
		
			inline KDTTri(const Point3f &va, const Point3f &vb, const Point3f &vc)
						: a( va ), e1( vb - va ), e2( vc - va )
			{
			}
		
			inline KDTTri(const Array<Point3f> &v, const IndexTri &tri)
			{
				a = v[ tri.i[0] ];
				const Point3f &b = v[ tri.i[1] ];
				const Point3f &c = v[ tri.i[2] ];
				e1 = b - a;
				e2 = c - a;
			}
		
		
		
			bool raytrace(const KDSegment &seg, float &t, float &u, float &v) const
			{
				//Uses Moller and Trumbore's algorithm
		
				//edges
				Vector3f p = seg.direction.cross( e2 );
		
				//compute determinant of matrix
				float determinant = p.dot( e1 );
				if ( determinant == 0.0 )
				{
					return false;
				}
				float inverseDeterminant = 1.0 / determinant;
		
				//compute u
				Vector3f s = seg.a - a;
				float ru = s.dot( p )  *  inverseDeterminant;
				if ( ru < 0.0  ||  ru > 1.0 )
				{
					return false;
				}
		
				//compute v
				Vector3f q = s.cross( e1 );
				float rv = seg.direction.dot( q )  *  inverseDeterminant;
				if ( rv < 0.0  ||  ( ru + rv ) > 1.0 )
				{
					return false;
				}
		
				//compute t
				float rt = e2.dot( q )  *  inverseDeterminant;
		
		
				if ( rt >= 0.0f  &&  rt <= 1.0f )
				{
					u = ru;
					v = rv;
					t = rt;
					return true;
				}
				else
				{
					return false;
				}
			}
		};

	private:
		class GS_DllExport KDTreeNode
		{
		public:
			BBox3f box;
			int startIndex, endIndex;
			int negativeIndex;		// bits 0 - 27 - index    28,29 - split axis index   30,31 - reserved
			int positiveIndex;		// bits 0 - 27 - index	28-31 - reserved
	

			KDTreeNode();


			void build(KDTree *tree, int nodeIndex, int start, int end, ProgressMonitor *kdtreeMonitor);
	

			bool intersects(const KDSegment &seg) const;
			bool isLeaf() const;
	

			inline int getSplitAxis() const
			{
				return ( negativeIndex & _KDT_SPLIT_AXIS_MASK )  >>  _KDT_SPLIT_AXIS_SHIFT;
			}

			inline void setSplitAxis(int axis)
			{
				negativeIndex = ( negativeIndex & _KDT_SPLIT_AXIS_INVMASK )  |  ( axis << _KDT_SPLIT_AXIS_SHIFT );
			}

			inline int getNegativeIndex() const
			{
				return negativeIndex & _KDT_NODE_INDEX_MASK;
			}

			inline void setNegativeIndex(int index)
			{
				negativeIndex = ( negativeIndex & _KDT_NODE_INDEX_INVMASK )  |  index;
			}
	
			inline int getPositiveIndex() const
			{
				return positiveIndex & _KDT_NODE_INDEX_MASK;
			}
	
			inline void setPositiveIndex(int index)
			{
				positiveIndex = ( positiveIndex & _KDT_NODE_INDEX_INVMASK )  |  index;
			}
	
			friend class KDTree;
		};
	
	

	private:
		BackgroundMesh *mesh;

		Array<KDTreeNode> nodes;
		Array<BBox3f> faceBoxes;
		Array<Point3f> faceCentroids;
		Array<int> faceOrder;
		int numNodesEstimate;

	
	public:
		KDTree();
		
		void readFromFile(BackgroundMesh *mesh, FILE *file);
		void writeToFile(FILE *file);

		void initialise(BackgroundMesh *mesh, ProgressMonitor *kdtreeMonitor);
	

	private:	
		int createNode(int startIndex, int endIndex, ProgressMonitor *kdtreeMonitor);
		

	
	private:
		int raytrace(const KDSegment &originalSeg, KDSegment &seg, float &segT, const KDTreeNode &node) const;

	public:
		int raytrace(const KDSegment &seg, Point3f &p, float &t) const;


		friend class KDTreeNode;
	};



public:
	typedef Array<int> IndexFace;





private:
	Array<Point3f> vPosition;
	Array<Vector3f> vNormal;
	Array<IndexTri> tris;

	enum
	{
		BUFFERINDEX_POSITION = 0,
		BUFFERINDEX_NORMAL = 1,
		BUFFERINDEX_INDICES = 2,
		NUMBUFFERS = 3
	};

#ifdef _BACKGROUNDMESH_USE_VERTEX_BUFFERS
	GLuint buffers[NUMBUFFERS];
#elif defined (_BACKGROUNDMESH_USE_VERTEX_ARRAYS_AND_DISPLAY_LIST)
	GLuint displayListId;
#endif
	bool bInitialisedGL;

	KDTree kdTree;


public:
	BackgroundMesh();
	BackgroundMesh(Array<Point3f> &inVerts, Array<IndexFace> &inFaces, ProgressMonitor *convertMonitor = NULL, ProgressMonitor *kdtreeMonitor = NULL);
	
	
	bool readFromFile(std::string filename);
	void writeToFile(std::string filename);


	void initGL();
	void drawGL();
	void shutdownGL();


	int raytrace(const Segment3f &seg, Point3f &p, float &t) const;


	boost::python::tuple py_raytrace(const Segment3f &seg) const;



	friend class KDTree;
};


#endif

