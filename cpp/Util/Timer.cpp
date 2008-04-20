//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef TIMER_CPP__
#define TIMER_CPP__

#include <stdio.h>

#include <Util/Timer.h>


#if defined(GSCULPT_PLATFORM_POSIX)
#include <sys/time.h>
#elif defined(GSCULPT_PLATFORM_WIN32)
#include <windows.h>
#endif




static double getCurrentTime()
{
	#if defined(GSCULPT_PLATFORM_POSIX)
		timeval t;
		gettimeofday( &t, NULL );
		return double( t.tv_sec )  +  double( t.tv_usec ) / 1000000.0;
	#elif defined(GSCULPT_PLATFORM_WIN32)
		return double( GetTickCount() ) / 1000.0;
	#else
		return 0.0;
	#endif
}





Timer::Timer() : startTime( 0.0 ), elapsedTime( 0.0 )
{
}


void Timer::start()
{
	startTime = getCurrentTime();
}

void Timer::stop()
{
	double stopTime = getCurrentTime();
	elapsedTime += stopTime - startTime;
	startTime = 0.0;
}

void Timer::reset()
{
	startTime = getCurrentTime();
	elapsedTime = 0.0;
}

double Timer::elapsed() const
{
	return elapsedTime;
}



#endif
