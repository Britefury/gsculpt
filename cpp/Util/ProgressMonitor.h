//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PROGRESSMONITOR_H__
#define PROGRESSMONITOR_H__

#include <stdlib.h>

template <typename ProgressType> class ProgressMonitor
{
public:
	virtual ~ProgressMonitor()
	{
	}
	
	virtual void updateProgress(const ProgressType &v) = 0;
};

#endif

