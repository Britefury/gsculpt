//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef FPUPRECISION_CPP__
#define FPUPRECISION_CPP__

#include <Util/FPUPrecision.h>



#if defined(GSCULPT_FPU_X86)

	#define FPU_PRECISION_MASK		0x0300
	#define FPU_PRECISION_SINGLE		0x0000
	#define FPU_PRECISION_DOUBLE		0x0200
	#define FPU_PRECISION_EXTENDED	0x0300

	static unsigned int getFPUState()
	{
		unsigned int control = 0;
		#if defined(_MSVC)
			__asm fnstcw control;
		#elif defined(__GNUG__)
			__asm__ __volatile__ ( "fnstcw %0" : "=m" ( control ) );
		#endif
		return control;
	}

	static void setFPUState(unsigned int control)
	{
		#if defined(_MSVC)
			__asm fldcw control;
		#elif defined(__GNUG__)
			__asm__ __volatile__ ( "fldcw %0" : : "m" ( control ) );
		#endif
	}

	static void modifyFPUState(unsigned int mask, unsigned int value)
	{
		unsigned int control = getFPUState();
		control = ( control & ~mask )  |  ( value & mask );
		setFPUState( control );
	}



	GS_DllExport void setFPUPrecision(FPUPrecision precision)
	{
		switch ( precision )
		{
		case FPUPRECISION_SINGLE:
			modifyFPUState( FPU_PRECISION_MASK, FPU_PRECISION_SINGLE );
			break;
		case FPUPRECISION_DOUBLE:
			modifyFPUState( FPU_PRECISION_MASK, FPU_PRECISION_DOUBLE );
			break;
		case FPUPRECISION_EXTENDED:
			modifyFPUState( FPU_PRECISION_MASK, FPU_PRECISION_EXTENDED );
			break;
		default:
			break;
		}
	}

#else

	GS_DllExport void setFPUPrecision(FPUPrecision precision)
	{
	}

#endif



#endif // FPUPRECISION_CPP__
