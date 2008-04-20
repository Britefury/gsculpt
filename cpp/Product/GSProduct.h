//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GSPRODUCT_H__
#define GSPRODUCT_H__

#include <string>
#include <map>

#include <Util/UniqueID.h>

#include <Util/SETree.h>

#include <RTType/RTType.h>





class GS_DllExport GSProduct : public RTObject
{
RT_TYPE;
public:
	class ObjectRepTable;

protected:
	class GS_DllExport ObjectRep : public RTObject
	{
	RT_TYPE;
	private:
		int paint3dCount, paintUVCount;

	protected:
		int refCount;

	public:
		ObjectRep();
		virtual ~ObjectRep();


	protected:
		static ObjectRep * readObjectRep(FILE *f, ObjectRepTable *repTable);
		static ObjectRep * readObjectRep(SENode &node, ObjectRepTable *repTable);
		static void writeObjectRep(SENode &node, ObjectRep *rep, ObjectRepTable *repTable);

		virtual void read(FILE *f, ObjectRepTable *repTable);
		virtual void write(FILE *f, ObjectRepTable *repTable);
		virtual void read(SENode &node, ObjectRepTable *repTable);
		virtual void write(SENode &node, ObjectRepTable *repTable);
		virtual void fillRepTable(ObjectRepTable *repTable);

		virtual int estimateCost() const;

	private:
		void ref();
		void unref();

		bool isUsed() const;
		bool isShared() const;

		void paint3dBeginNotify();
		void paint3dEndNotify();
		void paintUVBeginNotify();
		void paintUVEndNotify();

	protected:
		virtual void i_paint3dBeginNotify();
		virtual void i_paint3dEndNotify();
		virtual void i_paintUVBeginNotify();
		virtual void i_paintUVEndNotify();


		friend class GSProduct;
		friend class GSProduct::ObjectRepTable;
	};



public:
	class GS_DllExport ObjectRepTable
	{
	private:
		typedef std::map<UniqueID,ObjectRep*> IDToRepTab;
		typedef std::map<ObjectRep*,UniqueID> RepToIDTab;

		IDToRepTab idToRep;
		RepToIDTab repToId;
		UniqueID idCounter;

	public:
		ObjectRepTable();
		~ObjectRepTable();

		void clear();

		void read(FILE *f);
		void write(FILE *f);
		void read(SENode &node);
		void write(SENode &node);

	private:
		ObjectRep * getObjectRep(UniqueID id);
		UniqueID getObjectID(ObjectRep *rep);
		UniqueID addObjectRep(ObjectRep *rep);

		friend class GSProduct;
	};





private:
	ObjectRep *representation;
	bool objectLockedFlag, paint3dFlag, paintUVFlag;



public:
	GSProduct();
protected:
	GSProduct(ObjectRep *rep);
public:
	GSProduct(const GSProduct &o);
	virtual ~GSProduct();


	virtual std::string getDescription() const;


	virtual int estimateCost() const;


	virtual void copyFrom(const GSProduct *o);


	static GSProduct * readGSProduct(FILE *f, ObjectRepTable *repTable);
	static GSProduct * readGSProduct(SENode &node, ObjectRepTable *repTable);
	static void writeGSProduct(SENode &node, GSProduct *product, ObjectRepTable *repTable);
	static GSProduct * readSingleGSProduct(SENode &node);
	static void writeSingleGSProduct(SENode &node, GSProduct *product);

	virtual void read(FILE *f, ObjectRepTable *repTable);
	virtual void write(FILE *f, ObjectRepTable *repTable);
	virtual void read(SENode &node, ObjectRepTable *repTable);
	virtual void write(SENode &node, ObjectRepTable *repTable);
	void readSimple(SENode &node);
	void writeSimple(SENode &node);
	void fillRepTable(ObjectRepTable *repTable);




public:
	virtual void lock();
	virtual void unlock();
	void assertUnlocked();

	void paint3dBeginNotify();
	void paint3dEndNotify();
	void paintUVBeginNotify();
	void paintUVEndNotify();


protected:
	ObjectRep * getRepresentation();
	ObjectRep * getReadOnlyRepresentation();
	const ObjectRep * getReadOnlyRepresentation() const;

	virtual void applyLocalChangesToRepresentation(ObjectRep *rep);

	void unshareRepresentation();
	bool isRepresentationShared() const;

private:
	void setRepresentation(ObjectRep *newRepresentation);
	void unrefRepresentation();
	void refRepresentation();

	friend class ObjectRepTable;
};



#endif
