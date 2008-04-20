//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef RADIANS_H__
#define RADIANS_H__

#include <math.h>

#include <Math/MathConstants.h>


#ifndef DEGTORAD
#define DEGTORAD(a) (a * (M_PI / 180.0))
#endif

#ifndef RADTODEG
#define RADTODEG(a) (a * (180.0 / M_PI))
#endif


#endif
