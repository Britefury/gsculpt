//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GSPRODUCTMODEL_CPP__
#define GSPRODUCTMODEL_CPP__

#include <FileIO/fileio.h>

#include <Model/GSProductModel.h>




/***********************************************************************************
										GSProductModel::ModelRep
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( GSProductModel::ModelRep, GSProduct::ObjectRep );


GSProductModel::ModelRep::ModelRep() : ObjectRep()
{
}

GSProductModel::ModelRep::~ModelRep()
{
}



void GSProductModel::ModelRep::read(FILE *f, ObjectRepTable *repTable)
{
	ObjectRep::read( f, repTable );
}


void GSProductModel::ModelRep::write(FILE *f, ObjectRepTable *repTable)
{
	ObjectRep::write( f, repTable );
}


void GSProductModel::ModelRep::read(SENode &node, ObjectRepTable *repTable)
{
	ObjectRep::read( node[0], repTable );
}


void GSProductModel::ModelRep::write(SENode &node, ObjectRepTable *repTable)
{
	ObjectRep::write( node.addList(), repTable );
}


void GSProductModel::ModelRep::transform(const Matrix4 &xform)
{
}

void GSProductModel::ModelRep::markAll(bool markState)
{
}




/***********************************************************************************
												GSProductModel
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( GSProductModel, GSProduct );


GSProductModel::GSProductModel() : GSProduct( new ModelRep() )
{
	name = "";
	modelOrigin = Point3( 0.0, 0.0, 0.0 );
	xformLock = TRANSFORMATIONTYPE_NONLINEAR;

	bVirtualMirrorEnabled = false;

	localXform.flags.enabled = false;
	localXform.flags.saved = false;
	pointsSaved = false;
	modelOriginSaved = false;
	pointSaveMarkedOnly = false;

	markOverride.enabled = false;
	markOverride.enabledSave = false;
	markOverride.state = false;
	markOverride.stateSave = false;
	markSaved = false;
	markOverride.saved = false;
}

GSProductModel::GSProductModel(ObjectRep *rep) : GSProduct( rep )
{
	name = "";
	modelOrigin = Point3( 0.0, 0.0, 0.0 );
	xformLock = TRANSFORMATIONTYPE_NONLINEAR;

	bVirtualMirrorEnabled = false;

	localXform.flags.enabled = false;
	localXform.flags.saved = false;
	pointsSaved = false;
	modelOriginSaved = false;
	pointSaveMarkedOnly = false;

	markOverride.enabled = false;
	markOverride.enabledSave = false;
	markOverride.state = false;
	markOverride.stateSave = false;
	markSaved = false;
	markOverride.saved = false;
}

GSProductModel::GSProductModel(const GSProductModel &m) : GSProduct( m )
{
	name = m.name;
	modelOrigin = m.modelOrigin;
	xformLock = m.xformLock;

	virtualMirrorPlane = m.virtualMirrorPlane;
	bVirtualMirrorEnabled = m.bVirtualMirrorEnabled;

	localXform.flags.enabled = m.localXform.flags.enabled;
	localXform.xform = m.localXform.xform;
	localXform.xformInverse = m.localXform.xformInverse;

	markOverride.enabled = m.markOverride.enabled;
	markOverride.state = m.markOverride.state;


	localXform.flags.saved = false;
	pointsSaved = false;
	modelOriginSaved = false;
	pointSaveMarkedOnly = false;

	markOverride.enabledSave = false;
	markOverride.stateSave = false;
	markSaved = false;
	markOverride.saved = false;
}


GSProductModel::~GSProductModel()
{
}




std::string GSProductModel::getDescription() const
{
	return "Abstract Model";
}



int GSProductModel::estimateCost() const
{
	int superClassCost = GSProduct::estimateCost();

	return sizeof(GSProductModel) + superClassCost - sizeof(GSProduct);
}



void GSProductModel::copyFrom(const GSProduct *o)
{
	GSProduct::copyFrom( o );

	GSProductModel *model = (GSProductModel*)o;

	modelOrigin = model->modelOrigin;
	xformLock = model->xformLock;

	virtualMirrorPlane = model->virtualMirrorPlane;
	bVirtualMirrorEnabled = model->bVirtualMirrorEnabled;

	localXform.flags.enabled = model->localXform.flags.enabled;
	localXform.xform = model->localXform.xform;
	localXform.xformInverse = model->localXform.xformInverse;

	markOverride.enabled = model->markOverride.enabled;
	markOverride.state = model->markOverride.state;

	localXform.flags.saved = false;
	pointsSaved = false;
	modelOriginSaved = false;
	pointSaveMarkedOnly = false;

	markOverride.enabledSave = false;
	markOverride.stateSave = false;
	markSaved = false;
	markOverride.saved = false;
}




void GSProductModel::copyModelPropertiesFrom(GSProductModel *model)
{
	//only need to copy model origin, and transformation lock
	modelOrigin = model->modelOrigin;
	xformLock = model->xformLock;
}





void GSProductModel::read(FILE *f, ObjectRepTable *repTable)
{
	GSProduct::read( f, repTable );

	modelOrigin.read( f );

	name = readString( f );

	bool localXformEnabled;
	fread( &localXformEnabled, sizeof(bool), 1, f );
	localXform.flags.enabled = localXformEnabled;
	if ( localXform.flags.enabled )
	{
		localXform.xform.read( f );
		localXform.xformInverse = localXform.xform.inverse();
	}
	else
	{
		localXform.xform = Matrix4();
		localXform.xformInverse = Matrix4();
	}

	bool markOverrideEnabled;
	fread( &markOverrideEnabled, sizeof(bool), 1, f );
	markOverride.enabled = markOverrideEnabled;
	if ( markOverride.enabled )
	{
		bool state;
		fread( &state, sizeof(bool), 1, f );
		markOverride.state = state;
	}
}

void GSProductModel::write(FILE *f, ObjectRepTable *repTable)
{
	GSProduct::write( f, repTable );

	modelOrigin.write( f );

	writeString( f, name );

	bool localXformEnabled = localXform.flags.enabled;
	fwrite( &localXformEnabled, sizeof(bool), 1, f );
	if ( localXform.flags.enabled )
	{
		localXform.xform.write( f );
	}

	bool markOverrideEnabled = markOverride.enabled;
	fwrite( &markOverrideEnabled, sizeof(bool), 1, f );
	if ( markOverride.enabled )
	{
		bool state = markOverride.state;
		fwrite( &state, sizeof(bool), 1, f );
	}
}

void GSProductModel::read(SENode &node, ObjectRepTable *repTable)
{
	GSProduct::read( node[0], repTable );

	modelOrigin.read( node[1] );

	node[2]  >>  name;

	bool localXformEnabled;
	node[3]  >>  localXformEnabled;
	localXform.flags.enabled = localXformEnabled;
	if ( localXform.flags.enabled )
	{
		localXform.xform.read( node[4][0] );
		localXform.xformInverse = localXform.xform.inverse();
	}
	else
	{
		localXform.xform = Matrix4();
		localXform.xformInverse = Matrix4();
	}

	bool markOverrideEnabled;
	node[5]  >>  markOverrideEnabled;
	markOverride.enabled = markOverrideEnabled;
	if ( markOverride.enabled )
	{
		bool state = false;
		node[6][0]  >>  state;
		markOverride.state = state;
	}
}

void GSProductModel::write(SENode &node, ObjectRepTable *repTable)
{
	GSProduct::write( node.addList(), repTable );

	modelOrigin.write( node.addList() );

	node.addItem()  <<  name;

	bool localXformEnabled = localXform.flags.enabled;
	node.addItem()  <<  localXformEnabled;
	if ( localXform.flags.enabled )
	{
		localXform.xform.write( node.addList().addList() );
	}
	else
	{
		node.addList();
	}

	bool markOverrideEnabled = markOverride.enabled;
	node.addItem()  <<  markOverrideEnabled;
	if ( markOverride.enabled )
	{
		bool state = markOverride.state;
		node.addList().addItem()  <<  state;
	}
	else
	{
		node.addList();
	}
}



void GSProductModel::preComputeFull()
{
}



void GSProductModel::setModelOrigin(const Point3 &p)
{
	modelOrigin = p;
}

Point3 GSProductModel::getModelOrigin() const
{
	return modelOrigin;
}


void GSProductModel::saveModelOrigin()
{
	modelOriginSave = modelOrigin;
	modelOriginSaved = true;
}

void GSProductModel::restoreModelOrigin()
{
	if ( modelOriginSaved )
	{
		modelOrigin = modelOriginSave;
	}
}




VisualPlane & GSProductModel::getVirtualMirrorPlane()
{
	return virtualMirrorPlane;
}

bool GSProductModel::isVirtualMirrorEnabled() const
{
	return bVirtualMirrorEnabled;
}

void GSProductModel::setVirtualMirrorEnabled(bool bMirror)
{
	bVirtualMirrorEnabled = bMirror;
}





void GSProductModel::setName(const std::string &n)
{
	name = n;
}

const std::string &GSProductModel::getName() const
{
	return name;
}




bool GSProductModel::hasLocalTransformation() const
{
	return localXform.flags.enabled;
}

const Matrix4 &GSProductModel::getLocalTransformation() const
{
	return localXform.xform;
}

const Matrix4 &GSProductModel::getLocalTransformationInverse() const
{
	return localXform.xformInverse;
}



bool GSProductModel::hasMarkOverride() const
{
	return markOverride.enabled;
}

bool GSProductModel::getMarkOverride() const
{
	return markOverride.state;
}



void GSProductModel::flip()
{
}








Matrix4 GSProductModel::computeLockedTransformation(const Point3 &origin, const Vector3 &i, const Vector3 &j, const Vector3 &k) const
{
	switch( xformLock )
	{
	case TRANSFORMATIONTYPE_LINEAR:
		return Matrix4::translate( -modelOrigin.toVector3() )  *
				 Matrix4( i, j, k )  *
				 Matrix4::translate( origin.toVector3() );
	default:
		return Matrix4();
	}
}


void GSProductModel::transform(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly)
{
	if ( xformLock == TRANSFORMATIONTYPE_NONLINEAR  ||
		  xformLock == TRANSFORMATIONTYPE_LINEAR )
	{
		transformWithMatrix( m, transformOrigin, save, markedOnly );
	}
	else
	{
		Point3 origin = modelOrigin * m;
		Vector3 i = Vector3( 1.0, 0.0, 0.0 ) * m;
		Vector3 j = Vector3( 0.0, 1.0, 0.0 ) * m;
		Vector3 k = Vector3( 0.0, 0.0, 1.0 ) * m;

		transformWithMatrix( computeLockedTransformation( origin, i, j, k ), transformOrigin, save, markedOnly );
	}
}

void GSProductModel::transform(const Transformation *x, bool transformOrigin, bool save, bool markedOnly)
{
	if ( xformLock == TRANSFORMATIONTYPE_NONLINEAR )
	{
		applyFreeform( x, transformOrigin, save, markedOnly );
	}
	else
	{
		Point3 origin = x->transformPoint( modelOrigin );
		Vector3 i = x->transformVector( Vector3( 1.0, 0.0, 0.0 ), modelOrigin );
		Vector3 j = x->transformVector( Vector3( 0.0, 1.0, 0.0 ), modelOrigin );
		Vector3 k = x->transformVector( Vector3( 0.0, 0.0, 1.0 ), modelOrigin );

		transformWithMatrix( computeLockedTransformation( origin, i, j, k ), transformOrigin, save, markedOnly );
	}
}


void GSProductModel::transformUnlocked(const Matrix4 &m, bool transformOrigin, bool markedOnly)
{
	transformWithMatrix( m, transformOrigin, false, markedOnly );
}

void GSProductModel::transformUnlocked(const Transformation *x, bool transformOrigin, bool markedOnly)
{
	applyFreeform( x, transformOrigin, false, markedOnly );
}




void GSProductModel::transformRestore()
{
	if ( localXform.flags.saved )
	{
		localXform.xform = localXform.xformSave;
		localXform.flags.enabled = localXform.flags.enabledSave;
		localXform.xformInverse = localXform.xformInverseSave;
	}
	if ( pointsSaved )
	{
		unlock();
		i_transformPointsRestore( pointSaveMarkedOnly );
		lock();
	}
	if ( modelOriginSaved )
	{
		modelOrigin = modelOriginSave;
	}
}





void GSProductModel::transformWithMatrix(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly)
{
	MarkStatus markStatus = getMarkStatus();

	if ( markedOnly )
	{
		switch ( markStatus )
		{
		case MARKSTATUS_NONE:
			applyMatrixToLocalOrigin( m, transformOrigin, save );
			break;
		case MARKSTATUS_SOME:
			applyMatrix( m, transformOrigin, save, markedOnly );
			break;
		case MARKSTATUS_ALL:
			applyMatrixLocally( m, transformOrigin, save );
			break;
		default:
			break;
		}
	}
	else
	{
		applyMatrixLocally( m, transformOrigin, save );
	}
}


void GSProductModel::applyMatrixLocally(const Matrix4 &m, bool transformOrigin, bool save)
{
	if ( save )
	{
		localXform.xformSave = localXform.xform;
		localXform.flags.enabledSave = localXform.flags.enabled;
		localXform.xformInverseSave = localXform.xformInverse;

		if ( transformOrigin )
		{
			modelOriginSave = modelOrigin;
		}

		localXform.flags.saved = true;
		pointsSaved = false;
		modelOriginSaved = transformOrigin;
	}


	if ( transformOrigin )
	{
		modelOrigin *= m;
	}

	localXform.xform = localXform.xform * m;
	localXform.xformInverse = localXform.xformInverse * m.inverse();
	localXform.flags.enabled = true;
}


void GSProductModel::applyMatrixToLocalOrigin(const Matrix4 &m, bool transformOrigin, bool save)
{
	if ( save )
	{
		if ( transformOrigin )
		{
			modelOriginSave = modelOrigin;
		}

		localXform.flags.saved = false;
		pointsSaved = false;
		modelOriginSaved = transformOrigin;
	}

	if ( transformOrigin )
	{
		modelOrigin *= m;
	}
}


void GSProductModel::applyMatrix(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly)
{
	//unlock to ensure the rep is not shared, before saving the points
	unlock();

	if ( save )
	{
		i_transformPointsSave( markedOnly );

		if ( transformOrigin )
		{
			modelOriginSave = modelOrigin;
		}

		localXform.flags.saved = false;
		pointsSaved = true;
		modelOriginSaved = transformOrigin;
	}


	if ( transformOrigin )
	{
		modelOrigin *= m;
	}

	i_transformModel( m, transformOrigin, save, markedOnly );

	lock();
}

void GSProductModel::applyFreeform(const Transformation *x, bool transformOrigin, bool save, bool markedOnly)
{
	//unlock to ensure the rep is not shared, before saving the points
	unlock();

	if ( save )
	{
		i_transformPointsSave( markedOnly );

		if ( transformOrigin )
		{
			modelOriginSave = modelOrigin;
		}

		localXform.flags.saved = false;
		pointsSaved = true;
		modelOriginSaved = transformOrigin;
	}


	if ( transformOrigin )
	{
		modelOrigin = x->transformPoint( modelOrigin );
	}

	i_transformModel( x, transformOrigin, save, markedOnly );

	lock();
}


void GSProductModel::i_transformModel(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly)
{
	i_transform( m, markedOnly );
}

void GSProductModel::i_transformModel(const Transformation *x, bool transformOrigin, bool save, bool markedOnly)
{
	i_transform( x, markedOnly );
}

void GSProductModel::i_transform(const Matrix4 &m, bool markedOnly)
{
}

void GSProductModel::i_transform(const Transformation *x, bool markedOnly)
{
}



void GSProductModel::i_transformPointsSave(bool markedOnly)
{
	pointSaveMarkedOnly = markedOnly;
}

void GSProductModel::i_transformPointsRestore(bool markedOnly)
{
}



void GSProductModel::setTransformationLock(TransformationType l)
{
	xformLock = l;
}

TransformationType GSProductModel::getTransformationLock() const
{
	return xformLock;
}








void GSProductModel::markPoints(MarkPredicate predicate, const Region3d *region, bool save)
{
	if ( region != NULL )
	{
		if ( !region->isEmpty() )
		{
			Side side = arePointsInside( region );

			if ( side == SIDE_POSITIVE )
			{
				markAllPoints( predicate, save );
			}
			else if ( side == SIDE_NEGATIVE )
			{
				if ( predicate == MARKPREDICATE_MARK  ||  predicate == MARKPREDICATE_FILTER )
				{
					markAllPoints( MARKPREDICATE_UNMARK, save );
				}
			}
			else
			{
				applyMark( predicate, region, save );
			}
		}
	}
}

void GSProductModel::markAllPoints(MarkPredicate predicate, bool save)
{
	if ( predicate == MARKPREDICATE_ADD  ||  predicate == MARKPREDICATE_MARK  ||  predicate == MARKPREDICATE_UNMARK )
	{
		applyMarkOverride( predicate != MARKPREDICATE_UNMARK, save );
	}
	else
	{
		MarkStatus status = getMarkStatus();

		switch ( status )
		{
		case MARKSTATUS_NONE:
			applyMarkOverride( predicate == MARKPREDICATE_INVERT, save );
			return;
		case MARKSTATUS_ALL:
			applyMarkOverride( predicate == MARKPREDICATE_FILTER, save );
			return;
		case MARKSTATUS_SOME:
			//unlock before saving
			unlock();
			if ( save )
			{
				markSaved = true;
				i_markPointsSave();
			}
			i_markModelAll( predicate, save );
			lock();
			break;
		default:
			break;
		}
	}
}

void GSProductModel::markPointsRestore()
{
	if ( markSaved )
	{
		//unlock before restoring
		unlock();

		i_markPointsRestore();
		markSaved = false;

		lock();
	}
	if ( markOverride.saved )
	{
		markOverride.enabled = markOverride.enabledSave;
		markOverride.state = markOverride.stateSave;
	}
}

MarkStatus GSProductModel::getMarkStatus() const
{
	if ( markOverride.enabled )
	{
		return markOverride.state  ?  MARKSTATUS_ALL  :  MARKSTATUS_NONE;
	}
	else
	{
		return i_getMarkStatus();
	}
}

Side GSProductModel::arePointsInside(const Region3d *region) const
{
	if ( localXform.flags.enabled )
	{
		Region3d *r = region->cloneTyped<Region3d>();

		r->transform( localXform.xformInverse, localXform.xform );
		Side result = i_arePointsInside( r );
		delete r;

		return result;
	}
	else
	{
		return i_arePointsInside( region );
	}
}




void GSProductModel::applyMark(MarkPredicate predicate, const Region3d *region, bool save)
{
	//unlock() to ensure rep is not shared, before saving
	unlock();

	if ( save )
	{
		i_markPointsSave();

		markSaved = true;
		markOverride.saved = false;
	}

	i_markModel( predicate, region, save );

	lock();
}

void GSProductModel::applyMarkOverride(bool state, bool save)
{
	if ( save )
	{
		markOverride.enabledSave = markOverride.enabled;
		markOverride.stateSave = markOverride.state;

		markSaved = false;
		markOverride.saved = true;
	}

	markOverride.enabled = true;
	markOverride.state = state;
}



void GSProductModel::i_markModel(MarkPredicate predicate, const Region3d *region, bool save)
{
	i_markPoints( predicate, region );
}

void GSProductModel::i_markModelAll(MarkPredicate predicate, bool save)
{
	i_markAllPoints( predicate );
}

void GSProductModel::i_markPoints(MarkPredicate predicate, const Region3d *region)
{
}

void GSProductModel::i_markAllPoints(MarkPredicate predicate)
{
}

void GSProductModel::i_markPointsSave()
{
}

void GSProductModel::i_markPointsRestore()
{
}

MarkStatus GSProductModel::i_getMarkStatus() const
{
	return MARKSTATUS_NONE;
}

Side GSProductModel::i_arePointsInside(const Region3d *region) const
{
	return SIDE_NEGATIVE;
}







BBox3 GSProductModel::getBBox(bool markedOnly) const
{
	if ( markOverride.enabled  &&  markedOnly )
	{
		if ( markOverride.state )
		{
			markedOnly = false;
		}
		else
		{
			return BBox3();
		}
	}

	if ( localXform.flags.enabled )
	{
		return i_getBBox( localXform.xform, markedOnly );
	}
	else
	{
		return i_getBBox( markedOnly );
	}
}

BBox3 GSProductModel::getBBox(const Matrix4 &m, bool markedOnly) const
{
	if ( markOverride.enabled  &&  markedOnly )
	{
		if ( markOverride.state )
		{
			markedOnly = false;
		}
		else
		{
			return BBox3();
		}
	}

	if ( localXform.flags.enabled )
	{
		return i_getBBox( localXform.xform * m, markedOnly );
	}
	else
	{
		return i_getBBox( m, markedOnly );
	}
}

BBox3 GSProductModel::getBBox(const Transformation *x, bool markedOnly) const
{
	if ( markOverride.enabled  &&  markedOnly )
	{
		if ( markOverride.state )
		{
			markedOnly = false;
		}
		else
		{
			return BBox3();
		}
	}

	if ( localXform.flags.enabled )
	{
		return i_getBBox( localXform.xform, x, markedOnly );
	}
	else
	{
		return i_getBBox( x, markedOnly );
	}
}

BBox3 GSProductModel::getBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const
{
	if ( markOverride.enabled  &&  markedOnly )
	{
		if ( markOverride.state )
		{
			markedOnly = false;
		}
		else
		{
			return BBox3();
		}
	}

	if ( localXform.flags.enabled )
	{
		return i_getBBox( localXform.xform * m, x, markedOnly );
	}
	else
	{
		return i_getBBox( m, x, markedOnly );
	}
}



BBox3 GSProductModel::i_getBBox(bool markedOnly) const
{
	return BBox3();
}

BBox3 GSProductModel::i_getBBox(const Matrix4 &m, bool markedOnly) const
{
	return BBox3();
}

BBox3 GSProductModel::i_getBBox(const Transformation *x, bool markedOnly) const
{
	return BBox3();
}

BBox3 GSProductModel::i_getBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const
{
	return BBox3();
}



bool GSProductModel::raytrace(const Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersection)
{
	Segment3 tempRay = ray;
	return raytraceClipRay( tempRay, backfaceCullingFlag, t, intersection );
}

bool GSProductModel::raytraceClipRay(Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersection)
{
	if ( localXform.flags.enabled )
	{
		Segment3 tempRay = ray * localXform.xformInverse;
		bool result = i_raytraceClipRay( tempRay, backfaceCullingFlag, t, intersection );

		if ( result )
		{
			intersection *= localXform.xform;
			ray.b = intersection;
		}

		return result;
	}
	else
	{
		return i_raytraceClipRay( ray, backfaceCullingFlag, t, intersection );
	}
}


boost::python::tuple GSProductModel::py_raytrace(const Segment3 &ray, bool backfaceCullingFlag)
{
	double t;
	Point3 intersection;
	bool result = raytrace( ray, backfaceCullingFlag, t, intersection );
	return boost::python::make_tuple( result, t, intersection );
}

boost::python::tuple GSProductModel::py_raytraceClipRay(const Segment3 &ray, bool backfaceCullingFlag)
{
	double t;
	Point3 intersection;
	Segment3 r = ray;
	bool result = raytraceClipRay( r, backfaceCullingFlag, t, intersection );
	return boost::python::make_tuple( result, t, intersection, r );
}



bool GSProductModel::i_raytraceClipRay(Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersection)
{
	return false;
}


bool GSProductModel::canMergeWith(GSProductModel *withObject) const
{
	return false;
}

GSProductModel * GSProductModel::mergedWith(const Array<GSProductModel*> &models)
{
	// Ensure that merging can take place
	for (int modelI = 0; modelI < models.size(); modelI++)
	{
		if ( !canMergeWith( models[modelI] ) )
		{
			return NULL;
		}
	}

	GSProductModel *model0 = this;

	// Clone @this and unshare if necessary
	if ( model0->isRepresentationShared() )
	{
		model0 = model0->cloneTyped<GSProductModel>();
		model0->unshareRepresentation();
	}

	Array<GSProductModel*> unsharedModels;
	unsharedModels.resize( models.size() );
	for (int modelI = 0; modelI < models.size(); modelI++)
	{
		GSProductModel *m = models[modelI];

		if ( m->isRepresentationShared() )
		{
			m = m->cloneTyped<GSProductModel>();
			m->unshareRepresentation();
		}
		unsharedModels[modelI] = m;
	}


	GSProductModel *result = model0->i_mergedWith( unsharedModels );


	if ( model0 != this )
	{
		delete model0;
	}

	for (int modelI = 0; modelI < models.size(); modelI++)
	{
		if ( unsharedModels[modelI] != models[modelI] )
		{
			delete unsharedModels[modelI];
		}
	}


	return result;
}


GSProductModel * GSProductModel::py_mergedWith(boost::python::list pyModels)
{
	boost::python::object lenObj = pyModels.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	Array<GSProductModel*> models;
	if ( lenExtract.check() )
	{
		int numModels = lenExtract;
		models.reserve( numModels );
		for (int i = 0; i < numModels; i++)
		{
			boost::python::object modelObj = pyModels[i];
			boost::python::extract<GSProductModel*> modelExtract( modelObj );
			if ( modelExtract.check() )
			{
				models.push_back( modelExtract );
			}
		}
	}

	return mergedWith( models );
}



GSProductModel * GSProductModel::i_mergedWith(const Array<GSProductModel*> &models)
{
	return NULL;
}



void GSProductModel::applyLocalChangesToRepresentation(ObjectRep *rep)
{
	if ( localXform.flags.enabled )
	{
		ModelRep *mrep = (ModelRep*)rep;

		mrep->transform( localXform.xform );
		localXform.xform = Matrix4();
		localXform.xformInverse = Matrix4();
		localXform.flags.enabled = false;
	}

	if ( markOverride.enabled )
	{
		ModelRep *mrep = (ModelRep*)rep;

		mrep->markAll( markOverride.state );

		markOverride.state = false;
		markOverride.enabled = false;
	}
}


void GSProductModel::applyLocalChangesToModel(GSProductModel *model) const
{
	if ( hasLocalTransformation() )
	{
		const Matrix4 &m = getLocalTransformation();
		model->transform( m, true, false, false );
	}

	if ( hasMarkOverride() )
	{
		MarkPredicate pred = getMarkOverride()  ?  MARKPREDICATE_MARK  :  MARKPREDICATE_UNMARK;
		model->markAllPoints( pred, false );
	}
}


#endif
