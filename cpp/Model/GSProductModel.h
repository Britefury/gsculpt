//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GSPRODUCTMODEL_H__
#define GSPRODUCTMODEL_H__

#include <string>

#include <boost/python.hpp>

#include <Util/UniqueID.h>

#include <Math/Matrix4.h>
#include <Math/Segment3.h>
#include <Math/TransformationType.h>

#include <Transformation/Transformation.h>

#include <Graphics/Region.h>

#include <Model/MarkPredicate.h>
#include <Model/VisualPlane.h>

#include <Product/GSProduct.h>





class GS_DllExport GSProductModel : public GSProduct
{
RT_TYPE;
protected:
	class GS_DllExport ModelRep : public ObjectRep
	{
	RT_TYPE;
	public:
		ModelRep();
		virtual ~ModelRep();


	protected:
		virtual void read(FILE *f, ObjectRepTable *repTable);
		virtual void write(FILE *f, ObjectRepTable *repTable);
		virtual void read(SENode &node, ObjectRepTable *repTable);
		virtual void write(SENode &node, ObjectRepTable *repTable);

		virtual void transform(const Matrix4 &xform);
		virtual void markAll(bool markState);

		friend class GSProductModel;
	};


protected:
	std::string name;
	Point3 modelOrigin;

	VisualPlane virtualMirrorPlane;
	bool bVirtualMirrorEnabled;


private:
	Point3 modelOriginSave;
	bool modelOriginSaved;

	TransformationType xformLock;

	bool pointsSaved, pointSaveMarkedOnly;

	//local xform information
	struct
	{
		Matrix4 xform, xformSave;
		Matrix4 xformInverse, xformInverseSave;

		struct
		{
			unsigned enabled : 1;
			unsigned enabledSave : 1;
			unsigned saved : 1;
		} flags;
	} localXform;

	//mark override information
	struct
	{
		unsigned enabled : 1;
		unsigned enabledSave : 1;
		unsigned state : 1;
		unsigned stateSave : 1;
		unsigned saved : 1;
	} markOverride;
	bool markSaved;





public:
	GSProductModel();
protected:
	GSProductModel(ObjectRep *rep);
public:
	GSProductModel(const GSProductModel &m);
	virtual ~GSProductModel();


	virtual std::string getDescription() const;


	virtual int estimateCost() const;


	virtual void copyFrom(const GSProduct *o);


	void copyModelPropertiesFrom(GSProductModel *model);



	virtual void read(FILE *f, ObjectRepTable *repTable);
	virtual void write(FILE *f, ObjectRepTable *repTable);
	virtual void read(SENode &node, ObjectRepTable *repTable);
	virtual void write(SENode &node, ObjectRepTable *repTable);


	virtual void preComputeFull();



	void setModelOrigin(const Point3 &p);
	Point3 getModelOrigin() const;

	void saveModelOrigin();
	void restoreModelOrigin();


	VisualPlane & getVirtualMirrorPlane();
	bool isVirtualMirrorEnabled() const;
	void setVirtualMirrorEnabled(bool bMirror);


	void setName(const std::string &n);
	const std::string &getName() const;


protected:
	bool hasLocalTransformation() const;
	const Matrix4 &getLocalTransformation() const;
	const Matrix4 &getLocalTransformationInverse() const;

	bool hasMarkOverride() const;
	bool getMarkOverride() const;


public:
	virtual void flip();





private:
	Matrix4 computeLockedTransformation(const Point3 &origin, const Vector3 &i, const Vector3 &j, const Vector3 &k) const;

public:
	void transform(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly);
	void transform(const Transformation *x, bool transformOrigin, bool save, bool markedOnly);
	void transformUnlocked(const Matrix4 &m, bool transformOrigin, bool markedOnly);
	void transformUnlocked(const Transformation *x, bool transformOrigin, bool markedOnly);

	inline void py_transformMatrix(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly)
	{
		transform( m, transformOrigin, save, markedOnly );
	}

	inline void py_transformTransformation(const Transformation *x, bool transformOrigin, bool save, bool markedOnly)
	{
		transform( x, transformOrigin, save, markedOnly );
	}

	inline void py_transformUnlockedMatrix(const Matrix4 &m, bool transformOrigin, bool markedOnly)
	{
		transformUnlocked( m, transformOrigin, markedOnly );
	}

	inline void py_transformUnlockedTransformation(const Transformation *x, bool transformOrigin, bool markedOnly)
	{
		transformUnlocked( x, transformOrigin, markedOnly );
	}


	void transformRestore();


private:
	void transformWithMatrix(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly);

	void applyMatrixLocally(const Matrix4 &m, bool transformOrigin, bool save);
	void applyMatrixToLocalOrigin(const Matrix4 &m, bool transformOrigin, bool save);
	void applyMatrix(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly);
	void applyFreeform(const Transformation *x, bool transformOrigin, bool save, bool markedOnly);

protected:
	virtual void i_transformModel(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly);
	virtual void i_transformModel(const Transformation *x, bool transformOrigin, bool save, bool markedOnly);
	virtual void i_transform(const Matrix4 &m, bool markedOnly);
	virtual void i_transform(const Transformation *x, bool markedOnly);

	virtual void i_transformPointsSave(bool markedOnly);
	virtual void i_transformPointsRestore(bool markedOnly);

public:
	void setTransformationLock(TransformationType l);
	TransformationType getTransformationLock() const;





public:
	void markPoints(MarkPredicate predicate, const Region3d *region, bool save);
	virtual void markAllPoints(MarkPredicate predicate, bool save);
	virtual void markPointsRestore();

	virtual MarkStatus getMarkStatus() const;
	Side arePointsInside(const Region3d *region) const;


private:
	void applyMark(MarkPredicate predicate, const Region3d *region, bool save);
	void applyMarkOverride(bool state, bool save);

protected:
	virtual void i_markModel(MarkPredicate predicate, const Region3d *region, bool save);
	virtual void i_markModelAll(MarkPredicate predicate, bool save);
	virtual void i_markPoints(MarkPredicate predicate, const Region3d *region);
	virtual void i_markAllPoints(MarkPredicate predicate);
	virtual void i_markPointsSave();
	virtual void i_markPointsRestore();
	virtual MarkStatus i_getMarkStatus() const;
	virtual Side i_arePointsInside(const Region3d *region) const;




public:
	virtual BBox3 getBBox(bool markedOnly) const;
	virtual BBox3 getBBox(const Matrix4 &m, bool markedOnly) const;
	virtual BBox3 getBBox(const Transformation *x, bool markedOnly) const;
	virtual BBox3 getBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const;

	BBox3 py_getBBox(bool markedOnly) const
	{
		return getBBox( markedOnly );
	}

	BBox3 py_getBBoxMatrix(const Matrix4 &m, bool markedOnly) const
	{
		return getBBox( m, markedOnly );
	}

	BBox3 py_getBBoxTransformation(const Transformation *x, bool markedOnly) const
	{
		return getBBox( x, markedOnly );
	}

	BBox3 py_getBBoxMatrixAndTransformation(const Matrix4 &m, const Transformation *x, bool markedOnly) const
	{
		return getBBox( m, x, markedOnly );
	}


protected:
	virtual BBox3 i_getBBox(bool markedOnly) const;
	virtual BBox3 i_getBBox(const Matrix4 &m, bool markedOnly) const;
	virtual BBox3 i_getBBox(const Transformation *x, bool markedOnly) const;
	virtual BBox3 i_getBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const;


public:
	bool raytrace(const Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersection);
	bool raytraceClipRay(Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersection);

	// Returns: bool success, double t, Point3 intersection
	boost::python::tuple py_raytrace(const Segment3 &ray, bool backfaceCullingFlag);
	// Returns: bool success, double t, Point3 intersection, Segment3 clippedRay
	boost::python::tuple py_raytraceClipRay(const Segment3 &ray, bool backfaceCullingFlag);


protected:
	virtual bool i_raytraceClipRay(Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersection);


public:
	virtual bool canMergeWith(GSProductModel *withObject) const;
	virtual GSProductModel * mergedWith(const Array<GSProductModel*> &models);


	GSProductModel * py_mergedWith(boost::python::list pyModels);


protected:
	virtual GSProductModel * i_mergedWith(const Array<GSProductModel*> &models);



	virtual void applyLocalChangesToRepresentation(ObjectRep *rep);

	void applyLocalChangesToModel(GSProductModel *model) const;




public:
	friend class ModelPainter;
};


#endif
