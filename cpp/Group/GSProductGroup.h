/************************
 This program is free software; you can use it, redistribute it and/or modify it
 under the terms of the GNU General Public License version 2 as published by the
 Free Software Foundation. The full text of the GNU General Public License
 version 2 can be found in the file named 'COPYING' that accompanies this
 program. This source code is (C)copyright Geoffrey French 1999-2005.
 ************************/
#ifndef GSPRODUCTGROUP_H__
#define GSPRODUCTGROUP_H__

#include <boost/python.hpp>

#include <Util/Array.h>

#include <Model/GSProductModel.h>

#include <Graphics/Region.h>




class GS_DllExport GSProductGroup : public GSProductModel
{
RT_TYPE;
protected:
	class GS_DllExport GroupRep : public ModelRep
	{
	RT_TYPE;
	private:
		Array<GSProductModel*> objects;

	public:
		GroupRep();
		GroupRep(const GroupRep &r);
		virtual ~GroupRep();


	protected:
		virtual void read(FILE *f, ObjectRepTable *repTable);
		virtual void write(FILE *f, ObjectRepTable *repTable);
		virtual void read(SENode &node, ObjectRepTable *repTable);
		virtual void write(SENode &node, ObjectRepTable *repTable);
		virtual void fillRepTable(ObjectRepTable *repTable);

		virtual int estimateCost() const;


	protected:
		virtual void transform(const Matrix4 &xform);
		virtual void markAll(bool markState);


	public:
		void setObjectList(const Array<GSProductModel*> &objectList, bool cloneObjects);
		void addObject(GSProductModel *model, bool cloneObject);
		void addObjects(Array<GSProductModel*> &objectList, bool cloneObjects);
		void clear();

		int getNumObjects() const;
		Array<GSProductModel*> &getObjectList();
		GSProductModel * getObjectClone(int i) const;

		Array<GSProductModel*> &getObjectListForDisplay();

		void flip();


		void i_paint3dBeginNotify();
		void i_paint3dEndNotify();


		void i_transformModel(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly);
		void i_transformModel(const Transformation *x, bool transformOrigin, bool save, bool markedOnly);

		void i_transformPointsRestore(bool markedOnly);

		void i_markModel(MarkPredicate predicate, const Region3d *region, bool save);
		void i_markModelAll(MarkPredicate predicate, bool save);
		void i_markPointsRestore();
		MarkStatus i_getMarkStatus() const;
		Side i_arePointsInside(const Region3d *region) const;


		BBox3 i_getBBox(bool markedOnly) const;
		BBox3 i_getBBox(const Matrix4 &m, bool markedOnly) const;
		BBox3 i_getBBox(const Transformation *x, bool markedOnly) const;
		BBox3 i_getBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const;

		bool i_raytraceClipRay(Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersection);


		static void gatherContents(GSProductGroup *group, Array<GSProductModel*> &models);

		GSProductModel * mergeContents(GSProductGroup *group);

		friend class GSProductGroup;
	};



public:
	GSProductGroup();
protected:
	GSProductGroup(ObjectRep *rep);
public:
	GSProductGroup(const GSProductGroup &g);
	~GSProductGroup();


	virtual std::string getDescription() const;


	virtual void copyFrom(const GSProduct *o);



private:
	GroupRep* getGroupRepresentation();
	GroupRep* getReadOnlyGroupRepresentation();
	const GroupRep* getReadOnlyGroupRepresentation() const;

public:
	void setObjectList(const Array<GSProductModel*> &objectList, bool cloneObjects);
	void addObject(GSProductModel *model, bool cloneObject);
	void addObjects(Array<GSProductModel*> &objectList, bool cloneObjects);
	void clear();


	void py_append(GSProductModel *model);
	void py_extend(boost::python::list pyModels);


	int getNumModels() const;
	GSProductModel * getModelClone(int i);

	Array<GSProductModel*> & getObjectListForDisplay();

protected:
	Array<GSProductModel*> & getObjectList();

public:
	virtual void flip();


protected:
	virtual void i_transformModel(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly);
	virtual void i_transformModel(const Transformation *x, bool transformOrigin, bool save, bool markedOnly);

	virtual void i_transformPointsRestore(bool markedOnly);


	virtual void i_markModel(MarkPredicate predicate, const Region3d *region, bool save);
	virtual void i_markModelAll(MarkPredicate predicate, bool save);
	virtual void i_markPointsRestore();
	virtual MarkStatus i_getMarkStatus() const;
	virtual Side i_arePointsInside(const Region3d *region) const;


	virtual BBox3 i_getBBox(bool markedOnly) const;
	virtual BBox3 i_getBBox(const Matrix4 &m, bool markedOnly) const;
	virtual BBox3 i_getBBox(const Transformation *x, bool markedOnly) const;
	virtual BBox3 i_getBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const;

	virtual bool i_raytraceClipRay(Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersection);

public:
	virtual bool canMergeWith(GSProductModel *withObject) const;

protected:
	virtual GSProductModel * i_mergedWith(const Array<GSProductModel*> &models);




public:
	// ********************** GROUP SPECIFIC METHODS **********************
	GSProductModel * merged();


	friend class GroupRep;
};



#endif
