//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MBBTREE_H__
#define MBBTREE_H__

#include <Math/Point3.h>
#include <Math/BBox2.h>
#include <Math/BBox3.h>
#include <Math/Projection.h>
#include <Math/Side.h>

#include <Util/Array.h>
#include <Util/Pool.h>
#include <Util/BitList.h>

#include <Brush/Brush.h>

#include <Mesh/MMesh/MeshTypes.h>



/***************************************************************
							MBBTree
 ***************************************************************/

class GS_DllExport MBBTree
{
private:
	class BBTreeNode
	{
	private:
		int startIndex, endIndex;
		BBox3 box;
		Axis splitAxis;
		BBTreeNode *negative, *positive;

	public:
		BBTreeNode(MBBTree *tree, int start, int end);
		BBTreeNode(const BBTreeNode &node);
		~BBTreeNode();

		void * operator new(size_t sz);
		void operator delete(void *data);


		void refresh(MBBTree *tree);

		double queryCostHeuristic() const;


		bool intersects(const Segment3 &seg) const;
		Side projectIntoScreenSpace(const Projection &projection, BBox2 &box2) const;
		bool isWithinRange(const Point2 &screenPoint, double currentSqrDist, const Projection &projection) const;
		bool intersects(const Brush &b, const Point3 &brushPosition) const;
		bool isLeaf() const;

	private:
		static Pool<MBBTree::BBTreeNode> & getPool();

		friend class MBBTree;
	};


public:
	class GS_DllExport TraversalState
	{
	private:
		enum Order
		{
			ORDER_NEG_POS,
			ORDER_POS_NEG
		};

		enum Stage
		{
			STAGE_START,
			STAGE_SECOND_NODE,
			STAGE_FINISHED
		};

		class Entry
		{
		public:
			BBTreeNode *node;
			Order order;
			Stage stage;


			inline Entry()
					: node( NULL ), order( ORDER_NEG_POS ), stage( STAGE_START )
			{
			}

			inline Entry(BBTreeNode *node)
					: node( node ), order( ORDER_NEG_POS ), stage( STAGE_START )
			{
			}
		};

		Array<Entry> nodeStack;
		bool bInitialised;
		int numFaces;


	public:
		TraversalState();

		friend class MBBTree;
	};


private:
	MMesh *mesh;
	Array<BBox3> faceBoxes;
	Array<int> faceIndices;
	BitList modifiedFaces;
	bool bHasModifiedFaces, bTopologyModified;
	double initialCost;
	BBTreeNode *root;


public:
	MBBTree(MMesh *mesh);
	~MBBTree();


	void copyFrom(MBBTree *tree);


	void faceModified(int faceIndex);
	void topologyModified();


	void rebuild();
	void refresh();


	// To perform a ray trace operation, create a TraversalState object, and pass it to raytrace()
	// Keep calling raytrace() until it returns false, indicating that it has finished.
	// When raytrace() returns true, @start and @end will point the the start and end of an array of integers,
	// each of which is an index into the mesh face list.
	bool raytrace(const Segment3 &seg, TraversalState &state, const int *&start, const int *&end) const;

	bool findClosestScreenPoint(const Point2 &screenPoint, double currentSqrDist, const Projection &projection, TraversalState &state, const int *&start, const int *&end) const;

	bool brush(const Brush &b, const Point3 &brushPosition, TraversalState &state, const int *&start, const int *&end) const;



	friend class BBTreeNode;
};


#endif //MBBTREE_H__
