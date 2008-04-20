//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GSPRODUCTMODELBLANK_H__
#define GSPRODUCTMODEL_H__

#include <string>

#include <Product/GSProduct.h>





class GS_DllExport GSProductModelBlank : public GSProduct
{
RT_TYPE;
public:
	GSProductModelBlank();
protected:
	GSProductModelBlank(ObjectRep *rep);
public:
	GSProductModelBlank(const GSProductModelBlank &m);
	virtual ~GSProductModelBlank();


	virtual std::string getDescription() const;


	virtual int estimateCost() const;


	virtual void read(SENode &node, ObjectRepTable *repTable);
	virtual void write(SENode &node, ObjectRepTable *repTable);
};


#endif
