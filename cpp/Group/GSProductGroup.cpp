/************************
 This program is free software; you can use it, redistribute it and/or modify it
 under the terms of the GNU General Public License version 2 as published by the
 Free Software Foundation. The full text of the GNU General Public License
 version 2 can be found in the file named 'COPYING' that accompanies this
 program. This source code is (C)copyright Geoffrey French 1999-2005.
 ************************/
#ifndef GSPRODUCTGROUP_CPP__
#define GSPRODUCTGROUP_CPP__

#include <string>
#include <sstream>

#include <Group/GSProductGroup.h>





/***********************************************************************************
										GSProductGroup::GroupRep
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( GSProductGroup::GroupRep, GSProductModel::ModelRep );


GSProductGroup::GroupRep::GroupRep() : ModelRep()
{
}

GSProductGroup::GroupRep::GroupRep(const GroupRep &r) : ModelRep()
{
	objects.reserve( r.objects.size() );

	for (int i = 0; i < r.objects.size(); i++)
	{
		gs_assert( r.objects[i] != NULL, "GSProductGroup::GroupRep::GroupRep(): source objects should not be NULL\n" );

		gs_assert_is_instance_of( r.objects[i], GSProductModel, "GSProductGroup::GroupRep::GroupRep()" );

		objects.push_back( r.objects[i]->cloneTyped<GSProductModel>() );
	}
}

GSProductGroup::GroupRep::~GroupRep()
{
	clear();
}




void GSProductGroup::GroupRep::read(FILE *f, ObjectRepTable *repTable)
{
	ModelRep::read( f, repTable );

	int numObjects;
	fread( &numObjects, sizeof(int), 1, f );
	objects.reserve( numObjects );

	for (int i = 0; i < numObjects; i++)
	{
		GSProduct *obj = GSProduct::readGSProduct( f, repTable );

		gs_assert_is_instance_of( obj, GSProductModel, "GSProductGroup::GroupRep::read()" );

		objects.push_back( static_cast<GSProductModel*>( obj ) );
	}
}

void GSProductGroup::GroupRep::write(FILE *f, ObjectRepTable *repTable)
{
	ModelRep::write( f, repTable );

	int numObjects = objects.size();
	fwrite( &numObjects, sizeof(int), 1, f );

	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			objects[i]->write( f, repTable );
		}
	}
}

void GSProductGroup::GroupRep::read(SENode &node, ObjectRepTable *repTable)
{
	ModelRep::read( node[0], repTable );

	SENode &objectsNode = node[1];

	int numObjects = 0;
	objectsNode[0]  >>  numObjects;
	objects.reserve( numObjects );


	for (int i = 0; i < numObjects; i++)
	{
		GSProduct *obj = GSProduct::readGSProduct( objectsNode[i+1], repTable );

		if ( obj->isInstanceOf( GSProductModel::getTypeStatic() ) )
		{
			objects.push_back( static_cast<GSProductModel*>( obj ) );
		}
	}
}

void GSProductGroup::GroupRep::write(SENode &node, ObjectRepTable *repTable)
{
	ModelRep::write( node.addList(), repTable );

	SENode &objectsNode = node.addList();
	objectsNode.addItem()  <<  objects.size();
	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			objects[i]->write( objectsNode.addList(), repTable );
		}
	}
}



void GSProductGroup::GroupRep::fillRepTable(ObjectRepTable *repTable)
{
	ModelRep::fillRepTable( repTable );

	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			objects[i]->fillRepTable( repTable );
		}
	}
}



int GSProductGroup::GroupRep::estimateCost() const
{
	int superClassCost = ModelRep::estimateCost();

	int cumulativeCost = sizeof(GroupRep) + objects.computeMemoryUsage() + superClassCost - sizeof(ModelRep);


	for (int i = 0; i < objects.size(); i++)
	{
		cumulativeCost += objects[i]->estimateCost();
	}

	return cumulativeCost;
}






void GSProductGroup::GroupRep::transform(const Matrix4 &xform)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			GSProductModel *model = (GSProductModel*)objects[i];

			model->transform( xform, true, false, false );
		}
	}
}

void GSProductGroup::GroupRep::markAll(bool markState)
{
	MarkPredicate pred = markState  ?  MARKPREDICATE_MARK  :  MARKPREDICATE_UNMARK;
	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			GSProductModel *model = (GSProductModel*)objects[i];

			model->markAllPoints( pred, false );
		}
	}
}



void GSProductGroup::GroupRep::setObjectList(const Array<GSProductModel*> &objectList, bool cloneObjects)
{
	clear();

	if ( cloneObjects )
	{
		objects.reserve( objectList.size() );

		for (int i = 0; i < objectList.size(); i++)
		{
			gs_assert( objectList[i] != NULL, "GSProductGroup::GroupRep::setObjectList(): object list members cannot be NULL\n" );

			objects.push_back( (GSProductModel*)objectList[i]->clone() );
		}
	}
	else
	{
		objects = objectList;
	}
}

void GSProductGroup::GroupRep::addObject(GSProductModel *model, bool cloneObject)
{
	gs_assert( model != NULL, "GSProductGroup::GroupRep::addObject(): object list members cannot be NULL\n" );

	if ( cloneObject )
	{
		model = model->cloneTyped<GSProductModel>();
	}

	objects.push_back( model );
}

void GSProductGroup::GroupRep::addObjects(Array<GSProductModel*> &objectList, bool cloneObjects)
{
	for (int i = 0; i < objectList.size(); i++)
	{
		GSProductModel *model = objectList[i];

		gs_assert( model != NULL, "GSProductGroup::GroupRep::addObjects(): object list members cannot be NULL\n" );

		if ( cloneObjects )
		{
			model = model->cloneTyped<GSProductModel>();
		}

		objects.push_back( model );
	}
}

void GSProductGroup::GroupRep::clear()
{
	for (int i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
	objects.clear();
}




int GSProductGroup::GroupRep::getNumObjects() const
{
	return objects.size();
}

Array<GSProductModel*> &GSProductGroup::GroupRep::getObjectList()
{
	return objects;
}

GSProductModel* GSProductGroup::GroupRep::getObjectClone(int i) const
{
	const GSProductModel *object = objects[i];

	return (GSProductModel*)object->clone();
}


Array<GSProductModel*> &GSProductGroup::GroupRep::getObjectListForDisplay()
{
	return objects;
}





void GSProductGroup::GroupRep::flip()
{
	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			objects[i]->flip();
		}
	}
}



void GSProductGroup::GroupRep::i_paint3dBeginNotify()
{
	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			objects[i]->paint3dBeginNotify();
		}
	}
}

void GSProductGroup::GroupRep::i_paint3dEndNotify()
{
	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			objects[i]->paint3dEndNotify();
		}
	}
}



void GSProductGroup::GroupRep::i_transformModel(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			objects[i]->transform( m, true, save, markedOnly );
		}
	}
}

void GSProductGroup::GroupRep::i_transformModel(const Transformation *x, bool transformOrigin, bool save, bool markedOnly)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			objects[i]->transform( x, true, save, markedOnly );
		}
	}
}


void GSProductGroup::GroupRep::i_transformPointsRestore(bool markedOnly)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			objects[i]->transformRestore();
		}
	}
}


void GSProductGroup::GroupRep::i_markModel(MarkPredicate predicate, const Region3d *region, bool save)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			objects[i]->markPoints( predicate, region, save );
		}
	}
}

void GSProductGroup::GroupRep::i_markModelAll(MarkPredicate predicate, bool save)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			objects[i]->markAllPoints( predicate, save );
		}
	}
}

void GSProductGroup::GroupRep::i_markPointsRestore()
{
	for (int i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			objects[i]->markPointsRestore();
		}
	}
}

MarkStatus GSProductGroup::GroupRep::i_getMarkStatus() const
{
	MarkStatus status = MARKSTATUS_NONE;

	int i = 0;

	for (i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			status = objects[i]->getMarkStatus();
			break;
		}
	}

	if ( status == MARKSTATUS_SOME )
	{
		return MARKSTATUS_SOME;
	}

	for (; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			MarkStatus objectMarkStatus = objects[i]->getMarkStatus();

			if ( objectMarkStatus == MARKSTATUS_SOME )
			{
				return MARKSTATUS_SOME;
			}

			if ( objectMarkStatus != status )
			{
				return MARKSTATUS_SOME;
			}
		}
	}

	return status;
}

Side GSProductGroup::GroupRep::i_arePointsInside(const Region3d *region) const
{
	Side side = SIDE_NEGATIVE;

	int i;
	for (i = 0; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			side = objects[i]->arePointsInside( region );
			break;
		}
	}

	if ( side == SIDE_BOTH )
	{
		return SIDE_BOTH;
	}

	for (; i < objects.size(); i++)
	{
		if ( objects[i] != NULL )
		{
			Side objectSide = objects[i]->arePointsInside( region );

			if ( objectSide == SIDE_BOTH )
			{
				return SIDE_BOTH;
			}

			if ( objectSide != side )
			{
				return SIDE_BOTH;
			}
		}
	}

	return side;
}



BBox3 GSProductGroup::GroupRep::i_getBBox(bool markedOnly) const
{
	BBox3 box;
	bool gotBox = false;


	for (int i = 0; i < objects.size(); i++)
	{
		GSProductModel *model = objects[i];
		if ( model != NULL )
		{
			BBox3 modelBox = model->getBBox( markedOnly );

			if ( !modelBox.isEmpty() )
			{
				if ( gotBox )
				{
					box.addBox( modelBox );
				}
				else
				{
					box = modelBox;
					gotBox = true;
				}
			}
		}
	}


	return box;
}

BBox3 GSProductGroup::GroupRep::i_getBBox(const Matrix4 &m, bool markedOnly) const
{
	BBox3 box;
	bool gotBox = false;


	for (int i = 0; i < objects.size(); i++)
	{
		GSProductModel *model = objects[i];
		if ( model != NULL )
		{
			BBox3 modelBox = model->getBBox( m, markedOnly );

			if ( !modelBox.isEmpty() )
			{
				if ( gotBox )
				{
					box.addBox( modelBox );
				}
				else
				{
					box = modelBox;
					gotBox = true;
				}
			}
		}
	}


	return box;
}

BBox3 GSProductGroup::GroupRep::i_getBBox(const Transformation *x, bool markedOnly) const
{
	BBox3 box;
	bool gotBox = false;


	for (int i = 0; i < objects.size(); i++)
	{
		GSProductModel *model = objects[i];
		if ( model != NULL )
		{
			BBox3 modelBox = model->getBBox( x, markedOnly );

			if ( !modelBox.isEmpty() )
			{
				if ( gotBox )
				{
					box.addBox( modelBox );
				}
				else
				{
					box = modelBox;
					gotBox = true;
				}
			}
		}
	}


	return box;
}

BBox3 GSProductGroup::GroupRep::i_getBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const
{
	BBox3 box;
	bool gotBox = false;


	for (int i = 0; i < objects.size(); i++)
	{
		GSProductModel *model = objects[i];
		if ( model != NULL )
		{
			BBox3 modelBox = model->getBBox( m, x, markedOnly );

			if ( !modelBox.isEmpty() )
			{
				if ( gotBox )
				{
					box.addBox( modelBox );
				}
				else
				{
					box = modelBox;
					gotBox = true;
				}
			}
		}
	}


	return box;
}


bool GSProductGroup::GroupRep::i_raytraceClipRay(Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersection)
{
	bool hit = false;

	for (int i = 0; i < objects.size(); i++)
	{
		GSProductModel *model = objects[i];
		if ( model != NULL )
		{
			if ( model->raytraceClipRay( ray, backfaceCullingFlag, t, intersection ) )
			{
				hit = true;
			}
		}
	}

	return hit;
}



void GSProductGroup::GroupRep::gatherContents(GSProductGroup *group, Array<GSProductModel*> &models)
{
	group->unlock();
	GroupRep *rep = group->getGroupRepresentation();
	for (int objectI = 0; objectI < rep->objects.size(); objectI++)
	{
		GSProductModel *model = rep->objects[objectI]->cloneTyped<GSProductModel>();
		// Ensure @model does not share its representation
		model->unlock();
		model->lock();
		if ( model->isInstanceOf( GSProductGroup::getTypeStatic() ) )
		{
			gatherContents( static_cast<GSProductGroup*>( model ), models );
			// @model is no longer required
			delete model;
		}
		else
		{
			models.push_back( model );
		}
	}
	group->lock();
}


GSProductModel * GSProductGroup::GroupRep::mergeContents(GSProductGroup *group)
{
	// Clone @group, and gather a flattened list of contents
	GSProductGroup *groupClone = group->cloneTyped<GSProductGroup>();
	Array<GSProductModel*> models;
	gatherContents( groupClone, models );
	delete groupClone;



	// Generate a lists of models that can be merged with one another
	// Each list has the property that:
	//   The list consists of models [ M0, M1, ..... Mn ]:
	//	Each model M1 ... Mn can be merged with M0
	Array< Array<GSProductModel*> > mergeLists;
	for (int modelI = 0; modelI < models.size(); modelI++)
	{
		GSProductModel *model = models[modelI];

		// Try to find an existing merge list to insert @model into
		bool bFoundMergeList = false;
		for (int mergeListI = 0; mergeListI < mergeLists.size(); mergeListI++)
		{
			Array<GSProductModel*> &mergeList = mergeLists[mergeListI];
			GSProductModel *m0 = mergeList[0];
			if ( m0->canMergeWith( model ) )
			{
				// Found a merge list; append @model
				mergeList.push_back( model );
				bFoundMergeList = true;
				break;
			}
		}

		if ( !bFoundMergeList )
		{
			// Create a new merge list
			Array<GSProductModel*> &mergeList = mergeLists.push_back();
			mergeList.push_back( model );
		}
	}




	// Perform the merging
	Array<GSProductModel*> mergedModels;
	mergedModels.resize( mergeLists.size() );
	for (int mergeListI = 0; mergeListI < mergeLists.size(); mergeListI++)
	{
		Array<GSProductModel*> &mergeList = mergeLists[mergeListI];
		GSProductModel *m0 = mergeList[0];
		mergeList.remove( 0 );
		mergedModels[mergeListI] =m0->mergedWith( mergeList );
		mergedModels[mergeListI]->copyModelPropertiesFrom( group );

		delete m0;
		for (int modelI = 0; modelI < mergeList.size(); modelI++)
		{
			delete mergeList[modelI];
		}
	}


	if ( mergedModels.size() == 1 )
	{
		return mergedModels[0];
	}
	else
	{
		GSProductGroup *result = new GSProductGroup();
		result->setObjectList( mergedModels, false );
		result->copyModelPropertiesFrom( group );
		return result;
	}
}





/***********************************************************************************
												GSProductGroup
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( GSProductGroup, GSProductModel );


GSProductGroup::GSProductGroup() : GSProductModel( new GroupRep() )
{
}

GSProductGroup::GSProductGroup(ObjectRep *rep) : GSProductModel( rep )
{
}

GSProductGroup::GSProductGroup(const GSProductGroup &g) : GSProductModel( g )
{
}

GSProductGroup::~GSProductGroup()
{
	clear();
}



std::string GSProductGroup::getDescription() const
{
	std::ostringstream description;
	description << "Group: " << getNumModels() << " models" << (char)0x00;
	return description.str();
}



void GSProductGroup::copyFrom(const GSProduct *o)
{
	GSProductModel::copyFrom( o );
}





GSProductGroup::GroupRep* GSProductGroup::getGroupRepresentation()
{
	return (GroupRep*)getRepresentation();
}

GSProductGroup::GroupRep* GSProductGroup::getReadOnlyGroupRepresentation()
{
	return (GroupRep*)getReadOnlyRepresentation();
}

const GSProductGroup::GroupRep* GSProductGroup::getReadOnlyGroupRepresentation() const
{
	return (const GroupRep*)getReadOnlyRepresentation();
}



void GSProductGroup::setObjectList(const Array<GSProductModel*> &objectList, bool cloneObjects)
{
	unlock();
	GroupRep *rep = getGroupRepresentation();
	rep->setObjectList( objectList, cloneObjects );
	lock();
}

void GSProductGroup::addObject(GSProductModel *model, bool cloneObject)
{
	unlock();
	GroupRep *rep = getGroupRepresentation();
	rep->addObject( model, cloneObject );
	lock();
}

void GSProductGroup::addObjects(Array<GSProductModel*> &objectList, bool cloneObjects)
{
	unlock();
	GroupRep *rep = getGroupRepresentation();
	rep->addObjects( objectList, cloneObjects );
	lock();
}

void GSProductGroup::clear()
{
	unlock();
	GroupRep *rep = getGroupRepresentation();
	rep->clear();
	lock();
}



void GSProductGroup::py_append(GSProductModel *model)
{
	addObject( model, true );
}

void GSProductGroup::py_extend(boost::python::list pyModels)
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

	addObjects( models, true );
}



int GSProductGroup::getNumModels() const
{
	const GroupRep *rep = getReadOnlyGroupRepresentation();
	return rep->getNumObjects();
}

GSProductModel * GSProductGroup::getModelClone(int i)
{
	const GroupRep *rep = getReadOnlyGroupRepresentation();
	GSProductModel *model = rep->getObjectClone( i );

	applyLocalChangesToModel( model );

	return model;
}


Array<GSProductModel*> & GSProductGroup::getObjectListForDisplay()
{
	GroupRep *rep = getReadOnlyGroupRepresentation();
	return rep->getObjectListForDisplay();
}


Array<GSProductModel*> & GSProductGroup::getObjectList()
{
	unlock();
	GroupRep *rep = getGroupRepresentation();
	Array<GSProductModel*> &objectList = rep->getObjectList();
	lock();

	return objectList;
}



void GSProductGroup::flip()
{
	GSProductModel::flip();

	unlock();
	GroupRep *rep = getGroupRepresentation();
	rep->flip();
	lock();
}




void GSProductGroup::i_transformModel(const Matrix4 &m, bool transformOrigin, bool save, bool markedOnly)
{
	GSProductModel::i_transformModel( m, transformOrigin, save, markedOnly );

	GroupRep *rep = getGroupRepresentation();
	rep->i_transformModel( m, transformOrigin, save, markedOnly );
}

void GSProductGroup::i_transformModel(const Transformation *x, bool transformOrigin, bool save, bool markedOnly)
{
	GSProductModel::i_transformModel( x, transformOrigin, save, markedOnly );

	GroupRep *rep = getGroupRepresentation();
	rep->i_transformModel( x, transformOrigin, save, markedOnly );
}


void GSProductGroup::i_transformPointsRestore(bool markedOnly)
{
	GSProductModel::i_transformPointsRestore( markedOnly );

	GroupRep *rep = getGroupRepresentation();
	rep->i_transformPointsRestore( markedOnly );
}


void GSProductGroup::i_markModel(MarkPredicate predicate, const Region3d *region, bool save)
{
	GSProductModel::i_markModel( predicate, region, save );

	GroupRep *rep = getGroupRepresentation();
	rep->i_markModel( predicate, region, save );
}

void GSProductGroup::i_markModelAll(MarkPredicate predicate, bool save)
{
	GSProductModel::i_markModelAll( predicate, save );

	GroupRep *rep = getGroupRepresentation();
	rep->i_markModelAll( predicate, save );
}

void GSProductGroup::i_markPointsRestore()
{
	GSProductModel::i_markPointsRestore();

	GroupRep *rep = getGroupRepresentation();
	rep->i_markPointsRestore();
}

MarkStatus GSProductGroup::i_getMarkStatus() const
{
	const GroupRep *rep = getReadOnlyGroupRepresentation();
	return rep->i_getMarkStatus();
}

Side GSProductGroup::i_arePointsInside(const Region3d *region) const
{
	const GroupRep *rep = getReadOnlyGroupRepresentation();
	return rep->i_arePointsInside( region );
}


BBox3 GSProductGroup::i_getBBox(bool markedOnly) const
{
	const GroupRep *rep = getReadOnlyGroupRepresentation();
	return rep->i_getBBox( markedOnly );
}

BBox3 GSProductGroup::i_getBBox(const Matrix4 &m, bool markedOnly) const
{
	const GroupRep *rep = getReadOnlyGroupRepresentation();
	return rep->i_getBBox( m, markedOnly );
}

BBox3 GSProductGroup::i_getBBox(const Transformation *x, bool markedOnly) const
{
	const GroupRep *rep = getReadOnlyGroupRepresentation();
	return rep->i_getBBox( x, markedOnly );
}

BBox3 GSProductGroup::i_getBBox(const Matrix4 &m, const Transformation *x,
										bool markedOnly) const
{
	const GroupRep *rep = getReadOnlyGroupRepresentation();
	return rep->i_getBBox( m, x, markedOnly );
}


bool GSProductGroup::i_raytraceClipRay(Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersection)
{
	GroupRep *rep = getReadOnlyGroupRepresentation();
	return rep->i_raytraceClipRay( ray, backfaceCullingFlag, t, intersection );
}



bool GSProductGroup::canMergeWith(GSProductModel *withObject) const
{
	return withObject->isOfType( getType() );
}

GSProductModel * GSProductGroup::i_mergedWith(const Array<GSProductModel*> &models)
{
	//.Create a clone of @this, to merge the objects into
	GSProductGroup *destination = cloneTyped<GSProductGroup>();
	destination->unlock();

	// Merge the objects in @models into @destination
	for (int modelI = 0; modelI < models.size(); modelI++)
	{
		GSProductGroup *group = static_cast<GSProductGroup*>( models[modelI] );
		destination->addObjects( group->getObjectList(), true );
	}

	destination->lock();

	return destination;
}




GSProductModel * GSProductGroup::merged()
{
	//no need for GSProductGroup::GroupRep::mergeContents() to apply the local
	//transformation to its contents as the unlock()/lock() in
	//GSProductGroup::mergeContents() remove it
	unlock();
	GroupRep *rep = getGroupRepresentation();
	GSProductModel *merged = rep->mergeContents( this );
	lock();

	return merged;
}


#endif
